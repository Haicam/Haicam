package main

/*
#cgo android,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lhaicam-ifaddrs -lstdc++ -llog -landroid
#cgo linux,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lhaicam-ifaddrs -lstdc++
#cgo darwin,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lhaicam-ifaddrs -lstdc++
#cgo windows,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lhaicam-ifaddrs -static-libgcc -static-libstdc++
#include "haicam/P2Peer.h"
#include "haicam/discovery.h"
#include "ifaddrs.h"
*/
import "C"

import (
	_ "encoding/hex"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net"
	"net/http"
	"net/url"
	_ "os/exec"
	"sort"
	"strconv"
	"strings"
	"sync"
	"time"
	"unsafe"

	"github.com/beevik/etree"
	"github.com/google/uuid"
	"github.com/use-go/onvif"
	"github.com/use-go/onvif/device"
	"github.com/use-go/onvif/event"
	"github.com/use-go/onvif/gosoap"
	"github.com/use-go/onvif/media"
	discover "github.com/use-go/onvif/ws-discovery"
	"github.com/use-go/onvif/xsd"
	onv "github.com/use-go/onvif/xsd/onvif"
)

// Host struct
type Host struct {
	interfaceName string
	flags         net.Flags
	ip            net.IP
	mask          net.IPMask
}

var CallbackDelegateRef unsafe.Pointer

var running bool = false

const theadSleepTime = 5000

// GetLocalAddresses func
func GetLocalAddresses() *[]Host {
	hosts := make([]Host, 0)

	addrsJsonCStr := C.getInterfacesAndIPs()

	addrsJson := C.GoString(addrsJsonCStr)

	C.free(unsafe.Pointer(addrsJsonCStr))

	if len(addrsJson) > 2 { // for non android return "[]", fix android 11 net.Interfaces() permission issue
		var addrs []map[string]interface{}
		err := json.Unmarshal([]byte(addrsJson), &addrs)
		if err == nil {
			for _, addr := range addrs {
				if addr["isIPv4"].(float64) == 1 {
					ip, _, err := net.ParseCIDR(addr["ip"].(string) + "/24")
					if err == nil {
						hosts = append(hosts, Host{addr["interface"].(string), net.FlagUp, ip, net.IPv4Mask(255, 255, 255, 0)})
					}
				}
			}
		}
		return &hosts
	}

	ifaces, err := net.Interfaces()
	if err != nil {
		//localAddresses err: route ip+net: netlinkrib: permission denied for android 11
		Golog("localAddresses err: %+v", err.Error())
		return &hosts
	}

	for _, i := range ifaces {
		addrs, err := i.Addrs()
		if err != nil {
			Golog("localAddresses err: %+v", err.Error())
			continue
		}
		for _, a := range addrs {

			switch v := a.(type) {
			case *net.IPAddr:
				Golog("interface %v : %s (%s)", i.Name, v, v.IP.DefaultMask())
			case *net.IPNet:
				{
					if ipv4Addr := a.(*net.IPNet).IP.To4(); ipv4Addr != nil {
						if i.Name != "lo" {
							Golog("interface %v : name %s [ ip %v mask %v]", i.Name, v, v.IP, v.Mask)
							hosts = append(hosts, Host{i.Name, i.Flags, v.IP, v.Mask})
						}
					} else {
						Golog("interface %v : %s", i.Name, v)
					}
				}
			default:
				Golog("interface %v : %s", i.Name, v)
			}

		}
	}

	return &hosts
}

func printResponse(resp *http.Response) {
	b, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		panic(err)
	}
	//401 This onvif request requires authentication information
	//400 The action requested requires authorization and the sender is not authorized
	fmt.Println("StatusCode: ", resp.StatusCode)
	fmt.Println(gosoap.SoapMessage(string(b)).StringIndent())
}

func foundOnvifCamera(urn string, brand string, model string, ip string, port int) {

	r_urn := C.CString(urn)
	r_brand := C.CString(brand)
	r_model := C.CString(model)
	r_ip := C.CString(ip)
	C.onOnvifCameraFound(CallbackDelegateRef, r_urn, r_brand, r_model, r_ip, C.int(port))
	C.free(unsafe.Pointer(r_urn))
	C.free(unsafe.Pointer(r_brand))
	C.free(unsafe.Pointer(r_model))
	C.free(unsafe.Pointer(r_ip))

}

func runDiscovery(interfaceName string, ipAddr string) {

	/*
		<env:Body>
			<d:ProbeMatches>
				<d:ProbeMatch>
					<wsadis:EndpointReference>
						<wsadis:Address>urn:uuid:aa85c000-6324-11b3-819e-5850ed1fe385</wsadis:Address>
					</wsadis:EndpointReference>
					<d:Types>dn:NetworkVideoTransmitter tds:Device</d:Types>
					<d:Scopes>onvif://www.onvif.org/type/video_encoder onvif://www.onvif.org/Profile/Streaming onvif://www.onvif.org/Profile/G onvif://www.onvif.org/Profile/T onvif://www.onvif.org/MAC/58:50:ed:1f:e3:85 onvif://www.onvif.org/hardware/DS-2CD2385G1-I onvif://www.onvif.org/name/HIKVISION%20DS-2CD2385G1-I onvif://www.onvif.org/location/city/hangzhou</d:Scopes>
					<d:XAddrs>http://192.168.124.40/onvif/device_service http://[fe80::5a50:edff:fe1f:e385]/onvif/device_service</d:XAddrs>
					<d:MetadataVersion>10</d:MetadataVersion>
				</d:ProbeMatch>
			</d:ProbeMatches>
		</env:Body>
	*/

	devices := discover.SendProbe(interfaceName, ipAddr, nil, []string{"dn:NetworkVideoTransmitter", "tds:Device"}, map[string]string{"dn": "http://www.onvif.org/ver10/network/wsdl"})
	for _, j := range devices {
		//Golog(gosoap.SoapMessage(string(j)).StringIndent())
		defer func() {
			if e := recover(); e != nil {
				Golog("panic: runDiscovery device info")
				Golog(string(j))
			}
		}()

		doc := etree.NewDocument()
		if err := doc.ReadFromString(j); err != nil {
			Golog("error %s", err)
		} else {
			matches := doc.Root().FindElements("./Body/ProbeMatches/ProbeMatch")

			for _, match := range matches {
				xaddr := match.SelectElement("XAddrs")
				urls := strings.Split(xaddr.Text(), " ") // may have multiple url
				for _, u := range urls {
					if len(u) < 5 {
						continue
					}

					uri, err := url.Parse(u)
					if err != nil {
						Golog("url %s", u)
						continue
					}

					ip := uri.Hostname()
					if strings.Contains(ip, ":") { // ipv6
						continue
					}

					port, err := strconv.Atoi(uri.Port())
					if err != nil {
						port = 80
					}

					path := uri.Path

					isHaicam := false
					if strings.Contains(path, "ezhomelabs") { // ignore Haicam
						isHaicam = true
					}

					scopeMap := make(map[string]string)

					//So, in order to know if the device claims Profile S support,
					//you need to check if the scope onvif://www.onvif.org/Profile/Streaming is present.
					scopesElement := match.SelectElement("Scopes")
					scopes := strings.Split(scopesElement.Text(), " ")
					for _, v := range scopes {
						v, _ = url.PathUnescape(v)
						parts := strings.Split(v, "/")
						size := len(parts)
						if size > 2 {
							scopeMap[strings.ToLower(parts[size-2])] = strings.ToLower(parts[size-1])
						}
					}

					profile := scopeMap["profile"]
					if profile != "streaming" || profile != "t" {
						// !!!!may other profile working too
						// not profile S (streaming), or profile T (t)
						// Profile S is designed for IP-based video systems.
						// A Profile S device (e.g., an IP network camera or video encoder) is one that can send video data over an IP network to a Profile S client.
						//continue
					}

					brand := scopeMap["name"]
					if strings.Contains(brand, "hikvision") {
						brand = "hikvision"
					}

					model := scopeMap["hardware"]

					if val, ok := scopeMap["model"]; ok {
						model = val
					}

					if isHaicam {
						brand = "haicam"
						model = ""
					}

					urn := match.FindElement("./EndpointReference/Address").Text()

					Golog(urn, brand, model, profile, ip, port, path, u)
					foundOnvifCamera(urn, brand, model, ip, port)
				}
			}

		}

	}

}

func discoverCameras() {
	for running == true {
		hosts := GetLocalAddresses()
		ethN := -2
		wlanN := -2
		for _, v := range *hosts {
			name := v.interfaceName

			if strings.HasPrefix(v.ip.String(), "0.0.") || strings.HasPrefix(v.ip.String(), "127.0.") || strings.HasPrefix(v.ip.String(), "169.254.") {
				continue
			}
			if (v.flags & net.FlagUp) == 0 {
				continue
			}

			aName := name
			if name == "eth0" {
				ethN++
				if ethN > -1 {
					aName = fmt.Sprintf("eth0:%v", ethN)
				}

			}
			if name == "wlan0" {
				wlanN++
				if wlanN > -1 {
					aName = fmt.Sprintf("wlan0:%v", wlanN)
				}
			}
			if v.ip != nil && v.ip.String() != "" {
				Golog("discover-> %s", aName)
				go runDiscovery(name, v.ip.String())
				time.Sleep(1000 * time.Millisecond)
				go hikvisionProbe(name, v.ip.String())
				time.Sleep(1000 * time.Millisecond)
				go dahuaProbe(name, v.ip.String())
				time.Sleep(1000 * time.Millisecond)
			}
		}
		time.Sleep(theadSleepTime * time.Millisecond)
	}
}

func formatMac(mac string) string {
	mac = strings.ReplaceAll(mac, ":", "")
	mac = strings.ReplaceAll(mac, "-", "")
	mac = strings.ToUpper(mac)
	return mac
}

func foundHikvisionOrDahua(brand string, mac string, ipv4 string, iPort int, isDhcp bool) {

	Golog("onvif foundHikvisionOrDahua %v %v %v %v %v", brand, mac, ipv4, iPort, isDhcp)
	r_brand := C.CString(brand)
	r_mac := C.CString(mac)
	r_ipv4 := C.CString(ipv4)
	r_isDhcp := 0
	if isDhcp {
		r_isDhcp = 1
	}
	C.onBrandCameraFound(CallbackDelegateRef, r_brand, r_mac, r_ipv4, C.int(iPort), C.int(r_isDhcp))
	C.free(unsafe.Pointer(r_brand))
	C.free(unsafe.Pointer(r_mac))
	C.free(unsafe.Pointer(r_ipv4))
	Golog("onvif C.onBrandCameraFound end")

}

func hikvisionProbe(interfaceName string, ipAddr string) {

	uuidStr := uuid.New().String()
	group := []net.IP{net.IPv4(239, 255, 255, 250)}

	devices := discover.SendUDPMulticastPort(`<?xml version="1.0" encoding="utf-8"?><Probe><Uuid>`+uuidStr+`</Uuid><Types>inquiry</Types></Probe>`, interfaceName, ipAddr, group, 0, 37020)
	for _, j := range devices {
		//fmt.Println(gosoap.SoapMessage(string(j)).StringIndent())
		defer func() {
			if e := recover(); e != nil {
				Golog("panic: hikvisionProbe device info")
				Golog(string(j))
			}
		}()

		doc := etree.NewDocument()
		err := doc.ReadFromString(j)
		if err != nil {
			Golog("error %s", err)
			continue
		}
		mac := doc.Root().FindElement("/ProbeMatch/MAC").Text()
		mac = formatMac(mac)
		ipv4 := doc.Root().FindElement("/ProbeMatch/IPv4Address").Text()
		port := doc.Root().FindElement("/ProbeMatch/HttpPort").Text()

		dhcp := "true"
		if doc.Root().FindElement("/ProbeMatch/DHCP") != nil {
			dhcp = doc.Root().FindElement("/ProbeMatch/DHCP").Text()
		}

		activated := "true"
		if doc.Root().FindElement("/ProbeMatch/Activated") != nil {
			activated = doc.Root().FindElement("/ProbeMatch/Activated").Text()
		}

		Golog(mac, ipv4, port, dhcp, activated)
		iPort := 80
		iPort, _ = strconv.Atoi(port)
		isDhcp := false
		isDhcp, _ = strconv.ParseBool(dhcp)
		foundHikvisionOrDahua("hikvision", mac, ipv4, iPort, isDhcp)
		Golog("hikvision done")
	}

	/*<?xml version="1.0" encoding="UTF-8"?>
	<ProbeMatch>
		<Uuid>e8ecfe09-90e4-44c0-842b-703f46b58c81</Uuid>
		<Types>inquiry</Types>
		<DeviceType>141945</DeviceType>
		<DeviceDescription>DS-2CD2385G1-I</DeviceDescription>
		<DeviceSN>DS-2CD2385G1-I20200414AAWRE33842150</DeviceSN>
		<CommandPort>8000</CommandPort>
		<HttpPort>80</HttpPort>
		<MAC>58-50-ed-1f-e3-85</MAC>
		<IPv4Address>192.168.124.40</IPv4Address>
		<IPv4SubnetMask>255.255.255.0</IPv4SubnetMask>
		<IPv4Gateway>192.168.124.1</IPv4Gateway>
		<IPv6Address>::</IPv6Address>
		<IPv6Gateway>::</IPv6Gateway>
		<IPv6MaskLen>64</IPv6MaskLen>
		<DHCP>true</DHCP>
		<AnalogChannelNum>0</AnalogChannelNum>
		<DigitalChannelNum>1</DigitalChannelNum>
		<SoftwareVersion>V5.6.2build 190701</SoftwareVersion>
		<DSPVersion>V7.3 build 190626</DSPVersion>
		<BootTime>1970-01-15 02:44:11</BootTime>
		<Encrypt>true</Encrypt>
		<ResetAbility>false</ResetAbility>
		<DiskNumber>0</DiskNumber>
		<Activated>true</Activated>
		<PasswordResetAbility>true</PasswordResetAbility>
		<PasswordResetModeSecond>true</PasswordResetModeSecond>
		<DetailOEMCode>1</DetailOEMCode>
		<SupportSecurityQuestion>true</SupportSecurityQuestion>
		<SupportHCPlatform>true</SupportHCPlatform>
		<HCPlatformEnable>flase</HCPlatformEnable>
		<IsModifyVerificationCode>true</IsModifyVerificationCode>
		<Salt>d8eebc414effc857bed4c3cc203c8b4413f60b00a9f4acc410084358d5fd3f91</Salt>
		<DeviceLock>flase</DeviceLock>
		<SDKOverTLSPort>8443</SDKOverTLSPort>
		<SDKServerStatus>true</SDKServerStatus>
		<SDKOverTLSServerStatus>true</SDKOverTLSServerStatus>
		<SupportMailBox>true</SupportMailBox>
	</ProbeMatch>*/
}

func dahuaProbe(interfaceName string, ipAddr string) {
	//https://medium.com/@irshadhasmat/golang-simple-json-parsing-using-empty-interface-and-without-struct-in-go-language-e56d0e69968

	group := []net.IP{net.IPv4(239, 255, 255, 251)}
	devices := discover.SendUDPMulticastPort(" \x00\x00\x00DHIP\x00\x00\x00\x00\x00\x00\x00\x00I\x00\x00\x00\x00\x00\x00\x00I\x00\x00\x00\x00\x00\x00\x00{ \"method\" : \"DHDiscover.search\", \"params\" : { \"mac\" : \"\", \"uni\" : 1 } }\n", interfaceName, ipAddr, group, 0, 37810)

	/*
		header
		20 00 00 00
		44 48 49 50
		00 00 00 00
		00 00 00 00
		ef 02 00 00
		00 00 00 00
		ef 02 00 00
		00 00 00 00
		tail
		0a 00
	*/
	for _, j := range devices {
		defer func() {
			if e := recover(); e != nil {
				Golog("panic: dahuaProbe device info")
				Golog(string(j))
			}
		}()
		if string(j[4:8]) == "DHIP" {
			//fmt.Printf("%s", hex.Dump([]byte(j[32:len(j)-2])))

			var result map[string]interface{}

			json.Unmarshal([]byte(j[32:len(j)-2]), &result)
			mac := result["mac"]
			mac = formatMac(mac.(string))
			params := result["params"].(map[string]interface{})
			deviceInfo := params["deviceInfo"].(map[string]interface{})
			port := deviceInfo["HttpPort"]
			deviceType := deviceInfo["DeviceClass"]
			if deviceType != "IPC" {
				// continue
			}
			ipv4Addr := deviceInfo["IPv4Address"].(map[string]interface{})
			ipv4 := ipv4Addr["IPAddress"]
			dhcp := ipv4Addr["DhcpEnable"]
			Golog("%T, %T, %T, %T", mac, ipv4, port, dhcp)
			foundHikvisionOrDahua("dahua", mac.(string), ipv4.(string), int(port.(float64)), dhcp.(bool))
			Golog("dahua done")
		}
	}

	/*
		{"mac":"08:ed:ed:87:42:7a","method":"client.notifyDevInfo","params":{"deviceInfo":{"AlarmInputChannels":0,"AlarmOutputChannels":0,"DeviceClass":"IPC","DeviceID":"","DeviceType":"IPC-HDW1431T1-S4","Find":"BC","FindVersion":1,"HttpPort":80,"IPv4Address":{"DefaultGateway":"192.168.124.1","DhcpEnable":true,"IPAddress":"192.168.124.17","SubnetMask":"255.255.255.0"},"IPv6Address":{"DefaultGateway":null,"DhcpEnable":false,"IPAddress":"\/0","LinkLocalAddress":"fe80::0aed:edff:fe87:427a\/64"},"Init":2714,"MachineName":"6C01E93PAG69A7D","Manufacturer":"Private","Port":37777,"RemoteVideoInputChannels":0,"SerialNo":"6C01E93PAG69A7D","UnLoginFuncMask":1,"Vendor":"Private","Version":"2.800.0000000.13.R","VideoInputChannels":1,"VideoOutputChannels":0}}}
	*/
}

type CameraData struct {
	Mac          string
	Adapter      string
	IsDHCP       bool
	Streams      []CameraStream
	FailedReason string
}

// CameraStream struct
type CameraStream struct {
	Token            string
	Name             string
	VideoEncoding    string
	AudioEncoding    string
	Width            int
	Height           int
	VideoProtocol    string
	VideoPort        int
	VideoPath        string
	SnapshotProtocol string
	SnapshotPort     int
	SnapshotPath     string
}

//export GenerateUUID
func GenerateUUID() *C.char {
	uuidStr := uuid.New().String()
	Golog("onvif get UUID: %v", uuidStr)

	return C.CString(uuidStr)
}

//export GetCameraStreams
func GetCameraStreams(_cameraUuid *C.char, _ip *C.char, _port C.int, _username *C.char, _password *C.char) {
	cameraUuid := C.GoString(_cameraUuid)
	ip := C.GoString(_ip)
	port := int(_port)
	username := C.GoString(_username)
	password := C.GoString(_password)
	go getCameraStreamsInfo(cameraUuid, ip, port, username, password)
}

func getCameraStreamsInfo(cameraUuid string, ip string, port int, username string, password string) {
	cameraData, code := getCameraStreams(ip, port, username, password)
	b, err := json.Marshal(cameraData)
	if err != nil {
		log.Println("error:", err)
	}

	//Golog("getCameraStreamsInfo", code, string(b))

	r_cameraUuid := C.CString(cameraUuid)
	r_ip := C.CString(ip)
	jsonStr := C.CString(string(b))
	C.onGetCameraStreams(CallbackDelegateRef, r_cameraUuid, r_ip, jsonStr, C.int(code))
	C.free(unsafe.Pointer(r_cameraUuid))
	C.free(unsafe.Pointer(r_ip))
	C.free(unsafe.Pointer(jsonStr))
}

// Error code
// 0 - Connect timeout
// 404 - ONVIF not avaiable
// 401 - Unauthorized
/* onvif-log
Golog("onvif-log:GetNetworkInterfacesResponse:%s: %s", ipv4, string(b0))
Golog("onvif-log:GetProfilesResponse:%s: %s", ipv4, string(b))
Golog("onvif-log:GetStreamUriResponse:%s: %s", ipv4, string(b1))
Golog("onvif-log:GetSnapshotUriResponse:%s: %s", ipv4, string(b2))
Golog("onvif-log:CreatePullPointSubscriptionResponse:%s: %s", ipv4, string(b1))
Golog("onvif-log:PullMessagesResponse:%s: %s", ipv4, string(b2))
*/
func getCameraStreams(ipv4 string, port int, username string, password string) (*CameraData, int) {
	cameraData := CameraData{}
	//cameraData.Streams = make([]CameraStream, 0)
	statusCode := 200

	defer func() {
		if e := recover(); e != nil {
			Golog("panic: getCameraStreams")
		}
	}()

	dev, err := onvif.NewDevice(fmt.Sprintf("%v:%v", ipv4, port))
	if err != nil {
		_errMsg := err.Error()
		if strings.Contains(err.Error(), "code: 0") {
			statusCode = 0
		} else if strings.Contains(err.Error(), "code: 401") {
			statusCode = 401
		} else if strings.Contains(err.Error(), "code: 404") {
			statusCode = 404
		} else {
			statusCode = -1
		}

		if username != "" {
			statusCode = 200
			// try again with username and password for some devices, may be not standard ONVIF
			dev, err = onvif.NewDeviceWithAuth(fmt.Sprintf("%v:%v", ipv4, port), username, password)
			if err != nil {
				Golog("onvif-log:getCameraStreams:%s: %s", ipv4, err.Error())
				if strings.Contains(err.Error(), "code: 0") {
					statusCode = 0
				} else if strings.Contains(err.Error(), "code: 401") {
					statusCode = 401
				} else if strings.Contains(err.Error(), "code: 404") {
					statusCode = 404
				} else {
					statusCode = -1
				}
				//panic: camera is not available at 192.168.3.10 or it does not support ONVIF services
				return &cameraData, statusCode
			}
		} else {
			Golog("onvif-log:getCameraStreams:%s: %s", ipv4, _errMsg)
			return &cameraData, statusCode
		}
	} else {
		if username != "" {
			dev.Authenticate(username, password)
		}
	}

	res, err := dev.CallMethod(media.GetProfiles{})

	if err != nil {
		Golog(err.Error())
		return &cameraData, -2
	}

	b, err := ioutil.ReadAll(res.Body)
	if err != nil {
		Golog(err.Error())
		return &cameraData, -2
	}

	Golog("onvif-log:GetProfilesResponse:%s: %s", ipv4, string(b))

	if res.StatusCode != 200 {
		doc := etree.NewDocument()
		err = doc.ReadFromBytes(b)
		if err == nil {
			//<env:Reason><env:Text xml:lang="en">The device is locked because of entering wrong username/password many times.Please try it after 30 minutes!</env:Text></env:Reason>
			reason := doc.Root().FindElement("./Body/Fault/Reason")
			if reason != nil {
				cameraData.FailedReason = reason.Text()
			}
			if cameraData.FailedReason == "" {
				cameraData.FailedReason = reason.SelectElement("Text").Text()
			}
		}

		return &cameraData, res.StatusCode
	}

	doc := etree.NewDocument()
	err = doc.ReadFromBytes(b)
	if err != nil {
		Golog(err.Error())
		return &cameraData, -2
	}

	profiles := doc.Root().FindElements("./Body/GetProfilesResponse/Profiles")

	for _, profile := range profiles {
		cameraStream := CameraStream{}

		cameraStream.Token = profile.SelectAttrValue("token", "unknown")
		cameraStream.Name = profile.SelectElement("Name").Text()
		videoEncoderConfiguration := profile.SelectElement("VideoEncoderConfiguration")
		cameraStream.VideoEncoding = videoEncoderConfiguration.SelectAttrValue("encoding", "")

		if cameraStream.VideoEncoding == "" { // Hikvision camera has a wrong Encoding value for h265?
			cameraStream.VideoEncoding = videoEncoderConfiguration.SelectElement("Encoding").Text()
		}

		cameraStream.VideoEncoding = strings.ToUpper(cameraStream.VideoEncoding)
		cameraStream.VideoEncoding = strings.ReplaceAll(cameraStream.VideoEncoding, ".", "")
		resolution := videoEncoderConfiguration.SelectElement("Resolution")
		cameraStream.Width, err = strconv.Atoi(resolution.SelectElement("Width").Text())
		if err != nil {
			return &cameraData, -2
		}
		cameraStream.Height, err = strconv.Atoi(resolution.SelectElement("Height").Text())
		if err != nil {
			return &cameraData, -2
		}

		audioEncoderConfiguration := profile.SelectElement("AudioEncoderConfiguration")
		if audioEncoderConfiguration != nil {
			cameraStream.AudioEncoding = audioEncoderConfiguration.SelectElement("Encoding").Text()
			cameraStream.AudioEncoding = strings.ToUpper(cameraStream.AudioEncoding)
		}

		// get rtsp url
		res, err = dev.CallMethod(media.GetStreamUri{
			StreamSetup: onv.StreamSetup{
				Stream:    onv.StreamType("RTP-Unicast"),
				Transport: onv.Transport{Protocol: onv.TransportProtocol("RTSP")},
			},
			ProfileToken: onv.ReferenceToken(cameraStream.Token),
		})

		b1, err := ioutil.ReadAll(res.Body)
		if err != nil {
			return &cameraData, -2
		}

		Golog("onvif-log:GetStreamUriResponse:%s: %s", ipv4, string(b1))

		if res.StatusCode != 200 {
			return &cameraData, res.StatusCode
		}

		doc1 := etree.NewDocument()
		err = doc1.ReadFromBytes(b1)
		if err != nil {
			return &cameraData, -2
		}

		u1 := doc1.Root().FindElement("./Body/GetStreamUriResponse/MediaUri/Uri").Text()
		uri, err := url.Parse(u1)
		cameraStream.VideoProtocol = uri.Scheme
		cameraStream.VideoPort, err = strconv.Atoi(uri.Port())
		if err != nil {
			cameraStream.VideoPort = 554
		}
		cameraStream.VideoPath = uri.Path
		if uri.RawQuery != "" {
			cameraStream.VideoPath += "?" + uri.RawQuery
		}

		// get snapshot url
		res, err = dev.CallMethod(media.GetSnapshotUri{
			ProfileToken: onv.ReferenceToken(cameraStream.Token),
		})

		b2, err := ioutil.ReadAll(res.Body)
		if err != nil {
			return &cameraData, -2
		}

		Golog("onvif-log:GetSnapshotUriResponse:%s: %s", ipv4, string(b2))

		if res.StatusCode != 200 {
			return &cameraData, res.StatusCode
		}

		doc2 := etree.NewDocument()
		err = doc2.ReadFromBytes(b2)
		if err != nil {
			return &cameraData, -2
		}

		u2 := doc2.Root().FindElement("./Body/GetSnapshotUriResponse/MediaUri/Uri").Text()
		uri, err = url.Parse(u2)
		cameraStream.SnapshotProtocol = uri.Scheme
		cameraStream.SnapshotPort, err = strconv.Atoi(uri.Port())
		if err != nil {
			cameraStream.SnapshotPort = 80
		}
		cameraStream.SnapshotPath = uri.Path
		if uri.RawQuery != "" {
			cameraStream.SnapshotPath += "?" + uri.RawQuery
		}

		cameraData.Streams = append(cameraData.Streams, cameraStream)
	}

	sort.Slice(cameraData.Streams, func(i, j int) bool {
		return cameraData.Streams[i].Height > cameraData.Streams[j].Height
	})

	// check mac and DHCP
	cameraData.Adapter = ""
	cameraData.Mac = ""
	cameraData.IsDHCP = true

	res0, err0 := dev.CallMethod(device.GetNetworkInterfaces{})
	b0, _ := ioutil.ReadAll(res0.Body)

	doc0 := etree.NewDocument()
	err0 = doc0.ReadFromBytes(b0)
	if err0 == nil {
		Golog("onvif-log:GetNetworkInterfacesResponse:%s: %s", ipv4, string(b0))

		//str0, _ := doc0.WriteToString()
		//Golog("GetNetworkInterfacesResponse output %+v %s", res0.StatusCode, gosoap.SoapMessage(str0).StringIndent())

		interfaces := doc0.Root().FindElements("./Body/GetNetworkInterfacesResponse/NetworkInterfaces")

		if interfaces != nil {
			for _, _interface := range interfaces {
				_token := _interface.SelectAttrValue("token", "unknown")
				info := _interface.SelectElement("Info")
				_mac := info.SelectElement("HwAddress").Text()
				ipv4Interface := _interface.SelectElement("IPv4")
				config := ipv4Interface.SelectElement("Config")
				cameraData.IsDHCP, _ = strconv.ParseBool(config.SelectElement("DHCP").Text())

				_address := ""
				if cameraData.IsDHCP {
					fromDHCP := config.SelectElement("FromDHCP")
					_address = fromDHCP.SelectElement("Address").Text()
				} else {
					manual := config.SelectElement("Manual")
					_address = manual.SelectElement("Address").Text()
				}
				if _address == ipv4 {
					cameraData.Adapter = _token
					cameraData.Mac = _mac
					break
				}
			}
		} else {

		}
	} else {

	}

	Golog("Network Interface Token: %s mac: %s, isDhcp: %v", cameraData.Adapter, cameraData.Mac, cameraData.IsDHCP)
	// end check ma and DHCP

	//b, err = json.Marshal(cameraData)
	//if err != nil {
	//	log.Println("error:", err)
	//}

	//Golog("getCameraStreamsInfo before return: ", statusCode, string(b))

	return &cameraData, statusCode
}

var pullEventPool map[string]bool = make(map[string]bool)
var pullEventMutex sync.Mutex

//export StopPullEvents
func StopPullEvents(key *C.char) {
	uuidKey := C.GoString(key)

	pullEventMutex.Lock()
	_, exist := pullEventPool[uuidKey]
	if exist {
		pullEventPool[uuidKey] = false
		Golog("Events: stop", uuidKey)
	}
	pullEventMutex.Unlock()
}

//export StartPullEvents
func StartPullEvents(_cameraId C.int, _cameraUuid *C.char, _ipv4 *C.char, _port C.int, _username *C.char, _password *C.char) *C.char {
	pullEventKey := uuid.New().String()

	Golog("JNA_start_pull_events StartPullEvents: enter")

	pullEventMutex.Lock()
	pullEventPool[pullEventKey] = true
	pullEventMutex.Unlock()

	cameraId := int(_cameraId)
	cameraUuid := C.GoString(_cameraUuid)
	ipv4 := C.GoString(_ipv4)
	port := int(_port)
	username := C.GoString(_username)
	password := C.GoString(_password)

	Golog("JNA_start_pull_events StartPullEvents: start")
	go startPullEvents(pullEventKey, cameraId, cameraUuid, ipv4, port, username, password)
	Golog("JNA_start_pull_events StartPullEvents: end")

	return C.CString(pullEventKey)
}

func startPullEvents(pullEventKey string, cameraId int, cameraUuid string, ipv4 string, port int, username string, password string) {
	var address, currentTime, terminationTime string
	var statusCode int = 200
	var b1 []byte
	doc1 := etree.NewDocument()
	var res *http.Response
	var p1 event.CreatePullPointSubscription

	Golog("Events: start", pullEventKey, cameraId, cameraUuid, ipv4, port)

	defer func() {
		if e := recover(); e != nil {
			Golog("panic: startPullEvents")

			pullEventMutex.Lock()
			delete(pullEventPool, pullEventKey)
			pullEventMutex.Unlock()

			r_camera_uuid := C.CString(cameraUuid)
			r_motionTimeStr := C.CString("n/a")
			r_motionStateStr := C.CString("n/a")
			C.onCameraEvent(CallbackDelegateRef, C.int(cameraId), r_camera_uuid, r_motionTimeStr, r_motionStateStr, -3)
			C.free(unsafe.Pointer(r_camera_uuid))
			C.free(unsafe.Pointer(r_motionTimeStr))
			C.free(unsafe.Pointer(r_motionStateStr))
		}
	}()

	dev, err := onvif.NewDevice(fmt.Sprintf("%v:%v", ipv4, port))
	if err != nil {
		Golog("startPullEvents error ip port: ", ipv4, port)
		_errMsg := err.Error()
		if strings.Contains(err.Error(), "code: 0") {
			statusCode = 0
		} else if strings.Contains(err.Error(), "code: 401") {
			statusCode = 401
		} else if strings.Contains(err.Error(), "code: 404") {
			statusCode = 404
		} else {
			statusCode = -1
		}

		if username != "" {
			statusCode = 200
			// try again with username and password for some devices, may be not standard ONVIF
			dev, err = onvif.NewDeviceWithAuth(fmt.Sprintf("%v:%v", ipv4, port), username, password)
			if err != nil {
				Golog("onvif-log:startPullEvents:%s: %s", ipv4, err.Error())

				if strings.Contains(err.Error(), "code: 0") {
					statusCode = 0
				} else if strings.Contains(err.Error(), "code: 401") {
					statusCode = 401
				} else if strings.Contains(err.Error(), "code: 404") {
					statusCode = 404
				} else {
					statusCode = -1
				}

				pullEventMutex.Lock()
				delete(pullEventPool, pullEventKey)
				pullEventMutex.Unlock()

				Golog(strconv.Itoa(statusCode))
				goto exit
			}
		} else {
			Golog("onvif-log:startPullEvents:%s: %s", ipv4, _errMsg)

			pullEventMutex.Lock()
			delete(pullEventPool, pullEventKey)
			pullEventMutex.Unlock()

			Golog(strconv.Itoa(statusCode))
			goto exit
		}
	} else {
		if username != "" {
			dev.Authenticate(username, password)
		}
	}

start:
	pullEventMutex.Lock()
	if !running || !pullEventPool[pullEventKey] {
		delete(pullEventPool, pullEventKey)
		pullEventMutex.Unlock()
		goto exit
	}
	pullEventMutex.Unlock()

	time.Sleep(theadSleepTime * time.Millisecond)
	p1 = event.CreatePullPointSubscription{InitialTerminationTime: "PT60S"}
	res, err = dev.CallMethod(p1)
	if err != nil {
		//Golog("CreatePullPointSubscription PT60S", err.Error())
		//go to defer func
	}

	Golog("Events: StatusCode", res.StatusCode)

	b1, err = ioutil.ReadAll(res.Body)

	if err != nil {
		statusCode = -2

		pullEventMutex.Lock()
		delete(pullEventPool, pullEventKey)
		pullEventMutex.Unlock()

		log.Println(strconv.Itoa(statusCode))
		goto exit
	}

	if res.StatusCode != 200 {
		statusCode = res.StatusCode

		Golog("onvif-log:CreatePullPointSubscriptionResponse:%s: %s", ipv4, string(b1))

		pullEventMutex.Lock()
		delete(pullEventPool, pullEventKey)
		pullEventMutex.Unlock()

		log.Println(strconv.Itoa(statusCode))
		goto exit
	}

	err = doc1.ReadFromBytes(b1)
	if err != nil {
		statusCode = -2

		pullEventMutex.Lock()
		delete(pullEventPool, pullEventKey)
		pullEventMutex.Unlock()

		log.Println(strconv.Itoa(statusCode))
		goto exit
	}

	if doc1.Root().FindElement("./Body/Fault") != nil {
		Golog("onvif-log:CreatePullPointSubscriptionResponse:%s: %s", ipv4, string(b1))

		reason := doc1.Root().FindElement("./Body/Fault/Reason/Text").Text()

		pullEventMutex.Lock()
		delete(pullEventPool, pullEventKey)
		pullEventMutex.Unlock()

		log.Println(reason)
		statusCode = -2
		goto exit
	}

	fmt.Println(gosoap.SoapMessage(string(b1)).StringIndent())

	address = doc1.Root().FindElement("./Body/CreatePullPointSubscriptionResponse/SubscriptionReference/Address").Text()
	currentTime = doc1.Root().FindElement("./Body/CreatePullPointSubscriptionResponse/CurrentTime").Text()
	terminationTime = doc1.Root().FindElement("./Body/CreatePullPointSubscriptionResponse/TerminationTime").Text()

	Golog("Events:", address, "currentTime: ", currentTime, terminationTime)

	for running {

		pullEventMutex.Lock()
		if !pullEventPool[pullEventKey] {
			delete(pullEventPool, pullEventKey)
			pullEventMutex.Unlock()
			Golog("Events: stopped", pullEventKey)
			break
		}
		pullEventMutex.Unlock()

		p2 := event.PullMessages{Timeout: "PT600S", MessageLimit: 500}
		res, err := dev.CallMethodDo(address, p2)

		if err != nil {
			goto start
		}

		Golog("Events: StatusCode", res.StatusCode)

		b2, err := ioutil.ReadAll(res.Body)
		if err != nil {
			goto start
		}

		Golog("onvif-log:PullMessagesResponse:%s: %s", ipv4, string(b2))

		doc2 := etree.NewDocument()
		err = doc2.ReadFromBytes(b2)
		if err != nil {
			goto start
		}

		currentTimeElement := doc2.Root().FindElement("./Body/PullMessagesResponse/CurrentTime")
		if currentTimeElement == nil {
			goto start
		}

		currentTime = currentTimeElement.Text()
		terminationTime = doc2.Root().FindElement("./Body/PullMessagesResponse/TerminationTime").Text()

		Golog("Events: PullMessagesResponse currentTime: ", currentTime, terminationTime)

		notificationMessages := doc2.Root().FindElements("./Body/PullMessagesResponse/NotificationMessage")

		var motionTimeStr string
		var motionStateStr string
		var statusChange bool = false
		for _, notificationMessage := range notificationMessages {
			topic := notificationMessage.SelectElement("Topic").Text()
			if strings.HasSuffix(topic, "/Motion") {
				messages := notificationMessage.SelectElement("Message").SelectElements("Message")
				for _, message := range messages {
					fmt.Println(gosoap.SoapMessage(string(b2)).StringIndent())
					motionTimeStr = message.SelectAttrValue("UtcTime", "2000-01-01T01:01:01Z")
					motionStateStr = message.FindElement("./Data/SimpleItem").SelectAttrValue("Value", "false")
					statusChange = true
					break
				}
				if statusChange {
					break
				}
			}
			if strings.HasSuffix(topic, "/MotionAlarm") {
				messages := notificationMessage.SelectElement("Message").SelectElements("Message")
				for _, message := range messages {
					fmt.Println(gosoap.SoapMessage(string(b2)).StringIndent())
					motionTimeStr = message.SelectAttrValue("UtcTime", "2000-01-01T01:01:01Z")
					motionStateStr = message.FindElement("./Data/SimpleItem").SelectAttrValue("Value", "false")
					statusChange = true
					break
				}
				if statusChange {
					break
				}
			}
			if strings.HasSuffix(topic, "/Tamper") {
				messages := notificationMessage.SelectElement("Message").SelectElements("Message")
				for _, message := range messages {
					fmt.Println(gosoap.SoapMessage(string(b2)).StringIndent())
					motionTimeStr = message.SelectAttrValue("UtcTime", "2000-01-01T01:01:01Z")
					motionStateStr = message.FindElement("./Data/SimpleItem").SelectAttrValue("Value", "false")
					statusChange = true
					break
				}
				if statusChange {
					break
				}
			}

		}

		Golog("Events: motion event: %v %v %v %v %v", cameraId, cameraUuid, statusChange, motionTimeStr, motionStateStr)
		if statusChange {
			r_camera_uuid := C.CString(cameraUuid)
			r_motionTimeStr := C.CString(motionTimeStr)
			r_motionStateStr := C.CString(motionStateStr)
			C.onCameraEvent(CallbackDelegateRef, C.int(cameraId), r_camera_uuid, r_motionTimeStr, r_motionStateStr, 200)
			C.free(unsafe.Pointer(r_camera_uuid))
			C.free(unsafe.Pointer(r_motionTimeStr))
			C.free(unsafe.Pointer(r_motionStateStr))
		}

	}

exit:

	if statusCode != 200 {
		r_camera_uuid := C.CString(cameraUuid)
		r_motionTimeStr := C.CString("n/a")
		r_motionStateStr := C.CString("n/a")
		C.onCameraEvent(CallbackDelegateRef, C.int(cameraId), r_camera_uuid, r_motionTimeStr, r_motionStateStr, C.int(statusCode))
		C.free(unsafe.Pointer(r_camera_uuid))
		C.free(unsafe.Pointer(r_motionTimeStr))
		C.free(unsafe.Pointer(r_motionStateStr))
	}

}

/*
return
1 - OK
2 - OK, and rebooted
3 - OK, but reboot failed
0 - Err
-1 - auth error
-2 - not support get interface
-3 - not support set IP
-4 - not suppport set GW
*/

//export SetIPAddress
func SetIPAddress(_cameraIPv4 *C.char, _port C.int, _username *C.char, _password *C.char, _ipv4 *C.char, _gateway *C.char, _usingDHCP C.int) (ret C.int) {
	ret = 0

	defer func() {
		if e := recover(); e != nil {
			ret = 0
			Golog("panic: SetIPAddress")
		}
	}()

	cameraIPv4 := C.GoString(_cameraIPv4)
	port := int(_port)

	username := C.GoString(_username)
	password := C.GoString(_password)
	ipv4 := C.GoString(_ipv4)
	gateway := C.GoString(_gateway)

	usingDHCP := true
	if _usingDHCP == 0 {
		usingDHCP = false
	}

	dev, err := onvif.NewDevice(fmt.Sprintf("%v:%v", cameraIPv4, port))
	if err != nil {
		Golog("SetIPAddress error ip port: ", cameraIPv4, port)
		Golog(err.Error())
		if username != "" {
			// try again with username and password for some devices, may be not standard ONVIF
			dev, err = onvif.NewDeviceWithAuth(fmt.Sprintf("%v:%v", cameraIPv4, port), username, password)
			if err != nil {
				return -1
			}
		} else {
			return -1
		}
	} else {
		if username != "" {
			dev.Authenticate(username, password)
		}
	}

	res, err := dev.CallMethod(device.GetNetworkInterfaces{})

	b, _ := ioutil.ReadAll(res.Body)

	doc := etree.NewDocument()
	err = doc.ReadFromBytes(b)
	if err != nil {
		panic("err")
	}

	str, _ := doc.WriteToString()
	Golog("GetNetworkInterfacesResponse output %+v %s", res.StatusCode, gosoap.SoapMessage(str).StringIndent())

	interfaces := doc.Root().FindElements("./Body/GetNetworkInterfacesResponse/NetworkInterfaces")

	if interfaces == nil {
		Golog("GetNetworkInterfacesResponse/NetworkInterfaces failed")
		return -2
	}

	mac := ""
	token := ""

	for _, _interface := range interfaces {
		_token := _interface.SelectAttrValue("token", "unknown")
		info := _interface.SelectElement("Info")
		_mac := info.SelectElement("HwAddress").Text()
		ipv4Interface := _interface.SelectElement("IPv4")
		config := ipv4Interface.SelectElement("Config")
		isDhcp := config.SelectElement("DHCP").Text()

		_address := ""
		if isDhcp == "true" {
			fromDHCP := config.SelectElement("FromDHCP")
			_address = fromDHCP.SelectElement("Address").Text()
		} else {
			manual := config.SelectElement("Manual")
			_address = manual.SelectElement("Address").Text()
		}
		if _address == cameraIPv4 {
			token = _token
			mac = _mac
			break
		}
	}

	Golog("Network Interface Toten: %s mac: %s", token, mac)

	if token == "" {
		Golog("GetNetworkInterfacesResponse not token found for the IP")
		return -2
	}

	//res, err = dev.CallMethod(device.GetNetworkDefaultGateway{})
	//b, _ = ioutil.ReadAll(res.Body)
	//Golog("GetNetworkDefaultGateway output %+v %s", res.StatusCode, gosoap.SoapMessage(b).StringIndent())

	if !usingDHCP { //TODO ? pls check this logic later
		res, err = dev.CallMethod(device.SetNetworkDefaultGateway{IPv4Address: onv.IPv4Address(gateway)})
		b, _ = ioutil.ReadAll(res.Body)

		doc = etree.NewDocument()
		err = doc.ReadFromBytes(b)
		if err != nil {
			panic("err")
		}

		str, _ = doc.WriteToString()
		Golog("SetNetworkDefaultGatewayResponse output %+v %s", res.StatusCode, gosoap.SoapMessage(str).StringIndent())

		response := doc.Root().FindElement("./Body/SetNetworkDefaultGatewayResponse")
		if response == nil {
			Golog("SetNetworkDefaultGatewayResponse failed")
			return -4
		}
	}

	res, err = dev.CallMethod(device.SetNetworkInterfaces{
		InterfaceToken: onv.ReferenceToken(token),
		NetworkInterface: onv.NetworkInterfaceSetConfiguration{
			Enabled: true,
			MTU:     1500,
			IPv4: onv.IPv4NetworkInterfaceSetConfiguration{
				Enabled: true,
				Manual: onv.PrefixedIPv4Address{
					Address:      onv.IPv4Address(ipv4),
					PrefixLength: 24,
				},
				DHCP: xsd.Boolean(usingDHCP),
			},
			IPv6: onv.IPv6NetworkInterfaceSetConfiguration{
				Enabled:            false,
				AcceptRouterAdvert: false,
				Manual: onv.PrefixedIPv6Address{
					Address:      onv.IPv6Address("::"), // Unspecified Address ::/128
					PrefixLength: 128,
				},
				DHCP: "Off",
			},
		},
	})

	b, _ = ioutil.ReadAll(res.Body)
	doc = etree.NewDocument()
	err = doc.ReadFromBytes(b)
	if err != nil {
		panic("err")
	}

	str, _ = doc.WriteToString()
	Golog("SetNetworkInterfacesResponse output %+v %s", res.StatusCode, gosoap.SoapMessage(str).StringIndent())

	setNetworkInterfacesResponse := doc.Root().FindElement("./Body/SetNetworkInterfacesResponse")
	if setNetworkInterfacesResponse == nil {
		Golog("SetNetworkInterfacesResponse failed")
		return -3
	}
	rebootNeeded := setNetworkInterfacesResponse.SelectElement("RebootNeeded").Text()
	Golog("rebootNeeded %s", rebootNeeded)

	if rebootNeeded == "true" {
		res, err = dev.CallMethod(device.SystemReboot{})
		b, _ = ioutil.ReadAll(res.Body)
		doc = etree.NewDocument()
		err = doc.ReadFromBytes(b)
		if err != nil {
			panic("err")
		}

		str, _ = doc.WriteToString()
		Golog("reboot output %+v %s", res.StatusCode, gosoap.SoapMessage(str).StringIndent())

		response := doc.Root().FindElement("./Body/SystemRebootResponse")
		if response == nil {
			Golog("reboot failed")
			return 3
		} else {
			return 2
		}
	} else {
		return 1
	}
}

//export DiscoveryRun
func DiscoveryRun(cDelegateRef unsafe.Pointer) {

	CallbackDelegateRef = cDelegateRef
	Golog("DiscoveryRun started ...")

	// ip alias https://tldp.org/HOWTO/pdf/IP-Alias.pdf
	//haicamStart()
	/*out, err := exec.Command("date").Output()
	if err != nil {
		log.Fatal(err)
	}
	Golog("The date is %s", out)*/

	running = true
	go discoverCameras()
}
