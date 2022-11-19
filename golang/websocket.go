package main

/*
#cgo android,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lhaicam-ifaddrs -lstdc++ -llog -landroid
#cgo linux,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lhaicam-ifaddrs -lstdc++
#cgo darwin,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lhaicam-ifaddrs -lstdc++
#cgo windows,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lhaicam-ifaddrs -lwinpthread -lstdc++ -static-libgcc -static-libstdc++ -static
#include "websocket.h"
*/
import "C"

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"time"
	"unsafe"

	"github.com/gorilla/websocket"
	reuse "github.com/libp2p/go-reuseport"
	"github.com/shirou/gopsutil/disk"
)

const (
	writeWait = 10 * time.Second
	// Time allowed to read the next pong message from the client.
	pongWait = 60 * time.Second
	// Send pings to client with this period. Must be less than pongWait.
	pingPeriod = (pongWait * 9) / 10
)

// Hop-by-hop headers. These are removed when sent to the backend.
// http://www.w3.org/Protocols/rfc2616/rfc2616-sec13.html
var hopHeaders = []string{
	"Connection",
	"Keep-Alive",
	"Proxy-Authenticate",
	"Proxy-Authorization",
	"Te", // canonicalized version of "TE"
	"Trailers",
	"Transfer-Encoding",
	"Upgrade",
}

var callbackSocketDelegateRef unsafe.Pointer

var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024 * 1024,
	WriteBufferSize: 1024 * 1024,
	CheckOrigin: func(r *http.Request) bool {
		return true
	},
}

type Socket struct {
	newMessage chan []byte
	connt      *websocket.Conn
}

var peerList map[uintptr]*Socket = make(map[uintptr]*Socket)

func reader(ws *websocket.Conn) {
	defer func() {
		message := []byte(`{"command":0, "action", "closeSocket"}`)
		C.onWebSocketData(callbackSocketDelegateRef, C.ulonglong(uintptr(unsafe.Pointer(ws))), unsafe.Pointer(&message[0]), C.int(len(message)))
		delete(peerList, uintptr(unsafe.Pointer(&ws)))
		ws.Close()
	}()
	//ws.SetReadLimit(5120)
	ws.SetReadDeadline(time.Now().Add(pongWait))
	ws.SetPongHandler(func(message string) error {
		ws.SetReadDeadline(time.Now().Add(pongWait))
		return nil
	})
	for {
		_, message, err := ws.ReadMessage()
		if err != nil {
			break
		}

		if len(message) > 0 {
			Golog("onWebSocketData:", string(message))
			C.onWebSocketData(callbackSocketDelegateRef, C.ulonglong(uintptr(unsafe.Pointer(ws))), unsafe.Pointer(&message[0]), C.int(len(message)))
		}
	}
}

func writer(ws *websocket.Conn) {
	socket := peerList[uintptr(unsafe.Pointer(ws))]

	pingTicker := time.NewTicker(pingPeriod)
	defer func() {
		message := []byte(`{"command":0, "action", "closeSocket"}`)
		C.onWebSocketData(callbackSocketDelegateRef, C.ulonglong(uintptr(unsafe.Pointer(ws))), unsafe.Pointer(&message[0]), C.int(len(message)))
		delete(peerList, uintptr(unsafe.Pointer(&ws)))

		pingTicker.Stop()
		ws.Close()
	}()
	for {
		select {
		case message, ok := <-socket.newMessage:
			ws.SetWriteDeadline(time.Now().Add(writeWait))
			if !ok {
				ws.WriteMessage(websocket.CloseMessage, []byte{})
				return
			}

			//w, err := ws.NextWriter(websocket.TextMessage)
			//if err != nil {
			//	return
			//}
			//w.Write(message)
			//w.Close()
			ws.WriteMessage(websocket.TextMessage, message)
		case <-pingTicker.C:
			ws.SetWriteDeadline(time.Now().Add(writeWait))
			if err := ws.WriteMessage(websocket.PingMessage, []byte{}); err != nil {
				return
			}
		}
	}
}

func delHopHeaders(header http.Header) {
	for _, h := range hopHeaders {
		header.Del(h)
	}
}

func copyHeader(dst, src http.Header) {
	for k, vv := range src {
		for _, v := range vv {
			dst.Add(k, v)
		}
	}
}

// https://gist.github.com/yowu/f7dc34bd4736a65ff28d

func homeRoute(res http.ResponseWriter, req *http.Request) {
	Golog("homeRoute")

	path := "/local_videos"
	if req.URL.Path != "" && req.URL.Path != "/" {
		path = req.URL.Path
	}

	client := http.Client{
		Timeout: 10 * time.Second,
	}

	resp, err := client.Get("https://youipcams.com" + path)

	if resp != nil {
		defer resp.Body.Close()
	}

	if err != nil {
		fmt.Fprintf(res, "Please make sure you have an internet connection or try again later!")
		return
	}

	body, err := ioutil.ReadAll(resp.Body)

	if err != nil {
		fmt.Fprintf(res, "Please make sure you have an internet connection or try again later!")
		return
	}

	delHopHeaders(resp.Header)
	copyHeader(res.Header(), resp.Header)
	res.WriteHeader(resp.StatusCode)

	// use "%s" to avoid html % escape issue
	fmt.Fprintf(res, "%s", string(body))
}

func pingRoute(res http.ResponseWriter, req *http.Request) {
	Golog("pingRoute")
	fmt.Fprintf(res, "%s", "OK")
}

func websocketRoute(res http.ResponseWriter, req *http.Request) {
	Golog("websocketRoute")
	ws, err := upgrader.Upgrade(res, req, nil)
	if err != nil {
		Golog("upgrade:", err)
		return
	}

	var socket Socket
	socket.newMessage = make(chan []byte)
	socket.connt = ws

	peerList[uintptr(unsafe.Pointer(ws))] = &socket

	ws.SetCloseHandler(func(code int, text string) error {
		Golog("websocket closed,  code:", strconv.Itoa(code), text)
		message := []byte(`{"command":0, "action", "closeSocket"}`)
		C.onWebSocketData(callbackSocketDelegateRef, C.ulonglong(uintptr(unsafe.Pointer(ws))), unsafe.Pointer(&message[0]), C.int(len(message)))

		delete(peerList, uintptr(unsafe.Pointer(&ws)))
		message = websocket.FormatCloseMessage(code, "")
		ws.WriteControl(websocket.CloseMessage, message, time.Now().Add(writeWait))
		return nil
	})

	go writer(ws)
	reader(ws)
}

//export WebSocketStart
func WebSocketStart(listenAddr *C.char, cDelegateRef unsafe.Pointer) {
	callbackSocketDelegateRef = cDelegateRef

	addr := C.GoString(listenAddr)

	http.HandleFunc("/ws", websocketRoute)
	http.HandleFunc("/ping", pingRoute)
	http.HandleFunc("/", homeRoute)

	port := "8099"
	parts := strings.Split(addr, ":")
	if len(parts) == 2 {
		port = parts[1]
	}

	if !strings.Contains(addr, "127.0.0.1") && !strings.Contains(addr, "localhost") && !strings.Contains(addr, "0.0.0.0") {
		go func() {
			server := &http.Server{Addr: "127.0.0.1:" + port, Handler: nil}
			ln, _ := reuse.Listen("tcp", server.Addr)
			server.Serve(ln)
		}()

	}

	Golog("WebSocketStart ...")

	//http.ListenAndServe(addr, nil)
	server := &http.Server{Addr: addr, Handler: nil}
	ln, _ := reuse.Listen("tcp", server.Addr)
	server.Serve(ln)
}

//export SendWebSocketData
func SendWebSocketData(goObjRef C.ulonglong, data unsafe.Pointer, length C.int) {
	slice := C.GoBytes(data, length) // copy
	if socket, ok := peerList[uintptr(goObjRef)]; ok {
		//Golog("SendWebSocketData", strconv.FormatUint(uint64(uintptr(goObjRef)), 10))
		go func() {
			select {
			case socket.newMessage <- slice:
			case <-time.After(1 * time.Second):
			}
		}()
	}
}

//export GetExePath
func GetExePath() *C.char {
	ex, err := os.Executable()
	if err == nil {
		return C.CString(ex)
	}

	exReal, err := filepath.EvalSymlinks(ex)
	if err != nil {
		panic(err)
	}

	return C.CString(exReal)
}

/*
type UsageStat struct {
	Path              string  `json:"path"`
	Fstype            string  `json:"fstype"`
	Total             uint64  `json:"total"`
	Free              uint64  `json:"free"`
	Used              uint64  `json:"used"`
	UsedPercent       float64 `json:"usedPercent"`
	InodesTotal       uint64  `json:"inodesTotal"`
	InodesUsed        uint64  `json:"inodesUsed"`
	InodesFree        uint64  `json:"inodesFree"`
	InodesUsedPercent float64 `json:"inodesUsedPercent"`
}
*/

//export DirectoryUsage
func DirectoryUsage(directory *C.char) *C.char {
	var ret string
	usageStat, err := disk.Usage(C.GoString(directory))
	if err != nil {
		ret = "{}"
	} else {
		b, err := json.Marshal(usageStat)
		if err != nil {
			ret = "{}"
		} else {
			ret = string(b)
		}
	}

	return C.CString(ret)
}
