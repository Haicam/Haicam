package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net"
	"net/http"
	"strconv"
	"strings"
	"net/url"
	"sort"

	"github.com/gofrs/uuid"
	"github.com/use-go/onvif"
	"github.com/use-go/onvif/device"
	"github.com/use-go/onvif/media"
	"github.com/use-go/onvif/event"
	"github.com/use-go/onvif/gosoap"
	discover "github.com/use-go/onvif/ws-discovery"
	onv "github.com/use-go/onvif/xsd/onvif"
	"github.com/beevik/etree"
)

func printResponse(resp *http.Response) {
	b, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		panic(err)
	}
	fmt.Println(gosoap.SoapMessage(string(b)).StringIndent())
}

func eventTest() {
	dev, err := onvif.NewDevice("192.168.8.239")
	if err != nil {
		panic(err)
	}
	dev.Authenticate("admin", "admin")

	//p := event.CreatePullPointSubscription{InitialTerminationTime: "PT600S"}
	//res, err := dev.CallMethod(p)

	p := event.PullMessages{Timeout: "PT600S", MessageLimit: 500}
	res, err := dev.CallMethodDo("http://192.168.8.239:80/onvif/Events/PullPoint_2020-11-12T09:03:10Z_960057274", p)

	printResponse(res)

}

func dhcpTest() {
	dev, err := onvif.NewDevice("192.168.8.239")
	if err != nil {
		panic(err)
	}
	dev.Authenticate("admin", "admin")

	res, err := dev.CallMethod(device.SetNetworkInterfaces{InterfaceToken: "eth0", NetworkInterface: onv.NetworkInterfaceSetConfiguration{
		Enabled: true,
		MTU:     1500,
		IPv4:    onv.IPv4NetworkInterfaceSetConfiguration{Enabled: true, DHCP: true},
	}})
	//res, err := dev.CallMethod(device.GetNetworkInterfaces{})

	printResponse(res)

	/*
			<soap-env:Body>
			<tds:SetNetworkInterfaces>
				<tds:InterfaceToken>eth0</tds:InterfaceToken>
				<tds:NetworkInterface>
					<onvif:Enabled>true</onvif:Enabled>
					<onvif:MTU>1500</onvif:MTU>
					<onvif:IPv4>
						<onvif:Enabled>true</onvif:Enabled>
						<onvif:Manual>
							<onvif:Address/>
							<onvif:PrefixLength>0</onvif:PrefixLength>
						</onvif:Manual>
						<onvif:DHCP>true</onvif:DHCP>
					</onvif:IPv4>
				</tds:NetworkInterface>
			</tds:SetNetworkInterfaces>
		</soap-env:Body>
	*/

}

func upnpProbe() {
	group := []net.IP{net.IPv4(239, 255, 255, 250)}

	devices := discover.SendUDPMulticastPort("M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: ssdp:discover\r\nMX: 10\r\nST: ssdp:all", "enp37s0", "192.168.1.222", group, 1025, 1900)
	for _, j := range devices {
		fmt.Println(gosoap.SoapMessage(string(j)).StringIndent())
	}
}

func hikvisionProbe() {
	uuidV4 := uuid.Must(uuid.NewV4()).String()
	group := []net.IP{net.IPv4(239, 255, 255, 250)}

	devices := discover.SendUDPMulticastPort(`<?xml version="1.0" encoding="utf-8"?><Probe><Uuid>`+uuidV4+`</Uuid><Types>inquiry</Types></Probe>`, "enp37s0", "192.168.1.222", group, 1024, 37020)
	for _, j := range devices {
		fmt.Println(gosoap.SoapMessage(string(j)).StringIndent())
	}
}

func dahuaProbe() {
	uuidV4 := uuid.Must(uuid.NewV4()).String()

	group := []net.IP{net.IPv4(239, 255, 255, 251)}
	devices := discover.SendUDPMulticastPort(" \x00\x00\x00DHIP\x00\x00\x00\x00\x00\x00\x00\x00I\x00\x00\x00\x00\x00\x00\x00I\x00\x00\x00\x00\x00\x00\x00{ \"method\" : \"DHDiscover.search\", \"params\" : { \"mac\" : \"\", \"uni\" : 1 } }\n", "enp37s0", "192.168.1.222", group, 0, 37810)
	fmt.Println(uuidV4)

	/* header
	20 00 00 00
	44 48 49 50
	00 00 00 00
	00 00 00 00
	ef 02 00 00
	00 00 00 00
	ef 02 00 00
	00 00 00 00
	*/
	for _, j := range devices {
		if string(j[4:8]) == "DHIP" {
			fmt.Println(string(j[32:]))
		}
		//fmt.Println(gosoap.SoapMessage(string(j)).StringIndent())
	}
}

func qSeeProbe() { // not working
	uuidV4 := uuid.Must(uuid.NewV4()).String()

	data := [140]byte{0x4d, 0x48, 0x45, 0x44, 0x08, 0x00, 0x01, 0x00, 0x01}

	group := []net.IP{net.IPv4(234, 55, 55, 55), net.IPv4(234, 55, 55, 56)}
	devices := discover.SendUDPMulticastPort(string(data[0:]), "enp37s0", "192.168.1.222", group, 0, 23456)
	fmt.Println(uuidV4)

	for _, j := range devices {
		fmt.Println(string(j[0:]))
		if string(j[0:4]) == "MHED" { // 240 length
			fmt.Println(string(j[0:]))
		}
		//fmt.Println(gosoap.SoapMessage(string(j)).StringIndent())
	}
}

func main() {
	//qSeeProbe()
	client()

	//runDiscovery("en0")

	//s := onvif.GetAvailableDevicesAtSpecificEthernetInterface("en0")
	//log.Printf("%s", s)

	//eventTest()

	//dhcpTest()

	//upnpProbe()
}

type CameraData struct {
	Mac              string
	Adapter          string
	IsDHCP           bool
	Streams          []CameraStream
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
// 192.168.125.46 root/abmabmabm1
// 192.168.125.137 admin/123456
func client() {
	

	ipv4 := "192.168.1.27"
	username := "admin"
	password := "123456"
	port := 80

	getCameraStreams(ipv4, port, username, password)

}

//Golog func
func Golog(f string, a ...interface{}) {
	var msg string

	if strings.Contains(f, "%") {
		msg = fmt.Sprintf(f, a...)
	} else {
		msg += f + " "
		msg += fmt.Sprintln(a...)
	}

	fmt.Println(msg)
}


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
		Golog("getCameraStreams error ip port: ", ipv4, port)
		Golog(err.Error())
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
				Golog(err.Error())
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
			return &cameraData, statusCode
		}
	} else {
		if username != "" {
			dev.Authenticate(username, password)
		}
	}

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

		if (cameraStream.VideoEncoding == "") {
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

	//b, err = json.Marshal(cameraData)
	//if err != nil {
	//	log.Println("error:", err)
	//}

	//Golog("getCameraStreamsInfo before return: ", statusCode, string(b))

	return &cameraData, statusCode
}

// Host host
type Host struct {
	URL  string `json:"url"`
	Name string `json:"name"`
}

func runDiscovery(interfaceName string) {
	var hosts []*Host
	devices := discover.SendProbe(interfaceName, "192.168.1.222", nil, []string{"dn:NetworkVideoTransmitter"}, map[string]string{"dn": "http://www.onvif.org/ver10/network/wsdl"})
	for _, j := range devices {
		fmt.Println(gosoap.SoapMessage(string(j)).StringIndent())
		/*doc := etree.NewDocument()
		if err := doc.ReadFromString(j); err != nil {
			log.Printf("error %s", err)
		} else {

			endpoints := doc.Root().FindElements("./Body/ProbeMatches/ProbeMatch/XAddrs")
			scopes := doc.Root().FindElements("./Body/ProbeMatches/ProbeMatch/Scopes")

			flag := false

			host := &Host{}

			for _, xaddr := range endpoints {
				xaddr := strings.Split(strings.Split(xaddr.Text(), " ")[0], "/")[2]
				host.URL = xaddr
			}
			if flag {
				break
			}
			for _, scope := range scopes {
				re := regexp.MustCompile(`onvif:\/\/www\.onvif\.org\/name\/[A-Za-z0-9-]+`)
				match := re.FindStringSubmatch(scope.Text())
				host.Name = path.Base(match[0])
			}

			hosts = append(hosts, host)


		}*/

	}

	bys, _ := json.Marshal(hosts)
	log.Printf("done %s", bys)
}
