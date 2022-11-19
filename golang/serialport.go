package main

/*
#cgo android,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lhaicam-ifaddrs -lstdc++ -llog -landroid
#cgo linux,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lhaicam-ifaddrs -lstdc++
#cgo darwin,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lhaicam-ifaddrs -lstdc++
#cgo windows,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lhaicam-ifaddrs -static-libgcc -static-libstdc++
#include "discovery.h"
*/
import "C"

import (
	"encoding/json"
	"unsafe"

	"go.bug.st/serial"
)

var port serial.Port
var connected bool = false
var buff []byte = make([]byte, 256)

//export GetZWavePortsList
func GetZWavePortsList() *C.char {
	ret := "[]"

	// Retrieve the port list
	ports, err := serial.GetPortsList()
	if err != nil {
		Golog(err.Error())
		return C.CString(ret)
	}
	if len(ports) == 0 {
		return C.CString(ret)
	}

	// Print the list of detected ports
	for _, port := range ports {
		Golog("Found Serial port: %v\n", port)
	}

	b, err := json.Marshal(ports)
	if err == nil {
		ret = string(b)
	}

	return C.CString(ret)
}

//export OpenZWavePort
func OpenZWavePort(portName *C.char) C.int {

	portNameStr := C.GoString(portName)

	// Open the first serial port detected at 9600bps N81
	mode := &serial.Mode{
		BaudRate: 115200,
		Parity:   serial.NoParity,
		DataBits: 8,
		StopBits: serial.OneStopBit,
	}

	var err error

	port, err = serial.Open(portNameStr, mode)
	if err != nil {
		Golog(err.Error())
		return 0
	}

	connected = true

	return 1
}

//export ZWaveWriteData
func ZWaveWriteData(data unsafe.Pointer, length C.int) C.int {

	if !connected {
		return 0
	}

	slice := C.GoBytes(data, length) // copy

	n, err := port.Write(slice)
	if err != nil {
		connected = false
		port.Close()
		Golog(err.Error())
	}

	return C.int(n)
}

//export ZWaveReadData
func ZWaveReadData(data *unsafe.Pointer) C.int {
	n, err := port.Read(buff)
	if err != nil {
		Golog(err.Error())
	}

	if n == 0 {
		connected = false
		port.Close()
	}

	*data = unsafe.Pointer(&buff[0])

	return C.int(n)
}
