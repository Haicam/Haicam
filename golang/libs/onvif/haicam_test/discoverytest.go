package main

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

	"github.com/beevik/etree"
	"github.com/gofrs/uuid"
	_ "github.com/mattn/go-sqlite3"
	"github.com/use-go/onvif"
	"github.com/use-go/onvif/device"
	"github.com/use-go/onvif/event"
	"github.com/use-go/onvif/gosoap"
	"github.com/use-go/onvif/media"
	discover "github.com/use-go/onvif/ws-discovery"
	onv "github.com/use-go/onvif/xsd/onvif"
)

// Host struct
type Host struct {
	interfaceName string
	ip            net.IP
	mask          net.IPMask
}

var privateIP string = ""

// GetLocalAddresses func
func GetLocalAddresses() *[]Host {
	hosts := make([]Host, 0)

	ifaces, err := net.Interfaces()
	if err != nil {
		log.Print(fmt.Errorf("localAddresses: %+v", err.Error()))
		return &hosts
	}

	for _, i := range ifaces {
		addrs, err := i.Addrs()
		if err != nil {
			log.Print(fmt.Errorf("localAddresses: %+v", err.Error()))
			continue
		}
		for _, a := range addrs {

			switch v := a.(type) {
			case *net.IPAddr:
				log.Printf("%v : %s (%s)", i.Name, v, v.IP.DefaultMask())
			case *net.IPNet:
				{
					if ipv4Addr := a.(*net.IPNet).IP.To4(); ipv4Addr != nil {
						if i.Name != "lo0" {
							//log.Printf("interface %v : name %s [ ip %v mask %v]", i.Name, v, v.IP, v.Mask)
							hosts = append(hosts, Host{i.Name, v.IP, v.Mask})
						}
					}
				}
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

func runDiscovery(interfaceName string) {

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

	devices := discover.SendProbe(interfaceName, nil, []string{"dn:NetworkVideoTransmitter", "tds:Device"}, map[string]string{"dn": "http://www.onvif.org/ver10/network/wsdl"})
	for _, j := range devices {
		//fmt.Println(gosoap.SoapMessage(string(j)).StringIndent())
		doc := etree.NewDocument()
		if err := doc.ReadFromString(j); err != nil {
			log.Printf("error %s", err)
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
						log.Printf("url %s", u)
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

					scopeMap := make(map[string]string)

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

					brand := scopeMap["name"]
					if strings.Contains(brand, "hikvision") {
						brand = "hikvision"
					}

					model := scopeMap["hardware"]

					urn := match.FindElement("./EndpointReference/Address").Text()

					log.Println(urn, brand, model, ip, port, path, u)
					FoundOnvifCamera(urn, brand, model, ip, port, path)
				}
			}

		}

	}

}

func discoverCameras() {
	hosts := GetLocalAddresses()
	for _, v := range *hosts {
		if privateIP == v.ip.String() {
			continue
		}
		if strings.HasPrefix(v.ip.String(), "0.0.") || strings.HasPrefix(v.ip.String(), "169.254.") {
			continue
		}
		log.Printf("-> %s %s %s", v.interfaceName, v.ip.String(), v.mask.String())
		runDiscovery(v.interfaceName)
	}
}

func formatMac(mac string) string {
	mac = strings.ReplaceAll(mac, ":", "")
	mac = strings.ReplaceAll(mac, "-", "")
	mac = strings.ToUpper(mac)
	return mac
}

func hikvisionProbe() {
	uuidV4 := uuid.Must(uuid.NewV4()).String()
	group := []net.IP{net.IPv4(239, 255, 255, 250)}

	devices := discover.SendUDPMulticastPort(`<?xml version="1.0" encoding="utf-8"?><Probe><Uuid>`+uuidV4+`</Uuid><Types>inquiry</Types></Probe>`, "en0", group, 0, 37020)
	for _, j := range devices {
		//fmt.Println(gosoap.SoapMessage(string(j)).StringIndent())
		doc := etree.NewDocument()
		err := doc.ReadFromString(j)
		if err != nil {
			log.Printf("error %s", err)
			continue
		}
		mac := doc.Root().FindElement("/ProbeMatch/MAC").Text()
		mac = formatMac(mac)
		ipv4 := doc.Root().FindElement("/ProbeMatch/IPv4Address").Text()
		port := doc.Root().FindElement("/ProbeMatch/HttpPort").Text()
		dhcp := doc.Root().FindElement("/ProbeMatch/DHCP").Text()
		activated := doc.Root().FindElement("/ProbeMatch/Activated").Text()
		log.Println(mac, ipv4, port, dhcp, activated)
		iPort := 80
		iPort, _ = strconv.Atoi(port)
		isDhcp := false
		isDhcp, _ = strconv.ParseBool(dhcp)
		FoundHikvisionOrDahua("hikvision", mac, ipv4, iPort, isDhcp)
		log.Printf("hikvision done")
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

func dahuaProbe() {
	//https://medium.com/@irshadhasmat/golang-simple-json-parsing-using-empty-interface-and-without-struct-in-go-language-e56d0e69968

	group := []net.IP{net.IPv4(239, 255, 255, 251)}
	devices := discover.SendUDPMulticastPort(" \x00\x00\x00DHIP\x00\x00\x00\x00\x00\x00\x00\x00I\x00\x00\x00\x00\x00\x00\x00I\x00\x00\x00\x00\x00\x00\x00{ \"method\" : \"DHDiscover.search\", \"params\" : { \"mac\" : \"\", \"uni\" : 1 } }\n", "en0", group, 0, 37810)

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
			log.Printf("%T, %T, %T, %T", mac, ipv4, port, dhcp)
			FoundHikvisionOrDahua("dahua", mac.(string), ipv4.(string), int(port.(float64)), dhcp.(bool))
			log.Printf("dahua done")
		}
	}

	/*
		{"mac":"08:ed:ed:87:42:7a","method":"client.notifyDevInfo","params":{"deviceInfo":{"AlarmInputChannels":0,"AlarmOutputChannels":0,"DeviceClass":"IPC","DeviceID":"","DeviceType":"IPC-HDW1431T1-S4","Find":"BC","FindVersion":1,"HttpPort":80,"IPv4Address":{"DefaultGateway":"192.168.124.1","DhcpEnable":true,"IPAddress":"192.168.124.17","SubnetMask":"255.255.255.0"},"IPv6Address":{"DefaultGateway":null,"DhcpEnable":false,"IPAddress":"\/0","LinkLocalAddress":"fe80::0aed:edff:fe87:427a\/64"},"Init":2714,"MachineName":"6C01E93PAG69A7D","Manufacturer":"Private","Port":37777,"RemoteVideoInputChannels":0,"SerialNo":"6C01E93PAG69A7D","UnLoginFuncMask":1,"Vendor":"Private","Version":"2.800.0000000.13.R","VideoInputChannels":1,"VideoOutputChannels":0}}}
	*/
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

func getCameraStreams(ipv4 string, port int, username string, password string) (*[]CameraStream, int) {
	cameraStreams := make([]CameraStream, 0)
	statusCode := 200

	dev, err := onvif.NewDevice(fmt.Sprintf("%v:%v", ipv4, port))
	if err != nil {
		//panic: camera is not available at 192.168.3.10 or it does not support ONVIF services
		return &cameraStreams, -1
	}

	if username != "" {
		dev.Authenticate(username, password)
	}

	res, err := dev.CallMethod(media.GetProfiles{})

	if res.StatusCode != 200 {
		return &cameraStreams, res.StatusCode
	}

	b, err := ioutil.ReadAll(res.Body)
	if err != nil {
		return &cameraStreams, -2
	}

	doc := etree.NewDocument()
	err = doc.ReadFromBytes(b)
	if err != nil {
		return &cameraStreams, -2
	}

	str, _ := doc.WriteToString()
	fmt.Println(gosoap.SoapMessage(str).StringIndent())

	profiles := doc.Root().FindElements("./Body/GetProfilesResponse/Profiles")

	for _, profile := range profiles {
		cameraStream := CameraStream{}
		cameraStream.Token = profile.SelectAttrValue("token", "unknown")
		cameraStream.Name = profile.SelectElement("Name").Text()
		videoEncoderConfiguration := profile.SelectElement("VideoEncoderConfiguration")
		cameraStream.VideoEncoding = videoEncoderConfiguration.SelectElement("Encoding").Text()
		cameraStream.VideoEncoding = strings.ToUpper(cameraStream.VideoEncoding)
		resolution := videoEncoderConfiguration.SelectElement("Resolution")
		cameraStream.Width, err = strconv.Atoi(resolution.SelectElement("Width").Text())
		if err != nil {
			return &cameraStreams, -2
		}
		cameraStream.Height, err = strconv.Atoi(resolution.SelectElement("Height").Text())
		if err != nil {
			return &cameraStreams, -2
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

		if res.StatusCode != 200 {
			return &cameraStreams, res.StatusCode
		}

		b1, err := ioutil.ReadAll(res.Body)
		if err != nil {
			return &cameraStreams, -2
		}

		doc1 := etree.NewDocument()
		err = doc1.ReadFromBytes(b1)
		if err != nil {
			return &cameraStreams, -2
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

		if res.StatusCode != 200 {
			return &cameraStreams, res.StatusCode
		}

		b2, err := ioutil.ReadAll(res.Body)
		if err != nil {
			return &cameraStreams, -2
		}

		doc2 := etree.NewDocument()
		err = doc2.ReadFromBytes(b2)
		if err != nil {
			return &cameraStreams, -2
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

		cameraStreams = append(cameraStreams, cameraStream)
	}

	sort.Slice(cameraStreams, func(i, j int) bool {
		return cameraStreams[i].Height > cameraStreams[j].Height
	})

	return &cameraStreams, statusCode
}

func pullEvents(ipv4 string, port int, username string, password string) {

	var address, currentTime, terminationTime string
	statusCode := 200

	dev, err := onvif.NewDevice(fmt.Sprintf("%v:%v", ipv4, port))
	if err != nil {
		//panic: camera is not available at 192.168.3.10 or it does not support ONVIF services
		statusCode = -1
		panic(strconv.Itoa(statusCode))
	}

	if username != "" {
		dev.Authenticate(username, password)
	}

start:
	p1 := event.CreatePullPointSubscription{InitialTerminationTime: "PT600S"}
	res, err := dev.CallMethod(p1)

	log.Println("StatusCode", res.StatusCode)

	if res.StatusCode != 200 {
		statusCode = res.StatusCode
		panic(strconv.Itoa(statusCode))
	}

	b1, err := ioutil.ReadAll(res.Body)
	if err != nil {
		statusCode = -2
		panic(strconv.Itoa(statusCode))
	}

	doc1 := etree.NewDocument()
	err = doc1.ReadFromBytes(b1)
	if err != nil {
		statusCode = -2
		panic(strconv.Itoa(statusCode))
	}

	if doc1.Root().FindElement("./Body/Fault") != nil {
		reason := doc1.Root().FindElement("./Body/Fault/Reason/Text").Text()
		panic(reason)
	}

	fmt.Println(gosoap.SoapMessage(string(b1)).StringIndent())

	address = doc1.Root().FindElement("./Body/CreatePullPointSubscriptionResponse/SubscriptionReference/Address").Text()
	currentTime = doc1.Root().FindElement("./Body/CreatePullPointSubscriptionResponse/CurrentTime").Text()
	terminationTime = doc1.Root().FindElement("./Body/CreatePullPointSubscriptionResponse/TerminationTime").Text()

	log.Println(address, "currentTime: ", currentTime, terminationTime)

	for {
		p2 := event.PullMessages{Timeout: "PT600S", MessageLimit: 500}
		res, _ := dev.CallMethodDo(address, p2)

		log.Println("StatusCode", res.StatusCode)

		b2, err := ioutil.ReadAll(res.Body)
		if err != nil {
			goto start
		}

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

		log.Println("PullMessagesResponse currentTime: ", currentTime, terminationTime)

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

		log.Printf("motion event: %v %v %v ", statusChange, motionTimeStr, motionStateStr)

	}

}

func setIPAddress(cameraIPv4 string, port int, username string, password string, ipv4 string, gateway string) {
	statusCode := 200

	dev, err := onvif.NewDevice(fmt.Sprintf("%v:%v", cameraIPv4, port))
	if err != nil {
		//panic: camera is not available at 192.168.3.10 or it does not support ONVIF services
		statusCode = -1
		panic(strconv.Itoa(statusCode))
	}

	if username != "" {
		dev.Authenticate(username, password)
	}

	res, err := dev.CallMethod(device.GetNetworkInterfaces{})

	bs, _ := ioutil.ReadAll(res.Body)
	log.Printf("GetNetworkInterfaces output %+v %s", res.StatusCode, bs)

	res, err := dev.CallMethod(device.GetNetworkDefaultGateway{})
	bs, _ := ioutil.ReadAll(res.Body)
	log.Printf("GetNetworkDefaultGateway output %+v %s", res.StatusCode, bs)
}

func main() {

	// ip alias https://tldp.org/HOWTO/pdf/IP-Alias.pdf
	//haicamStart()
	/*out, err := exec.Command("date").Output()
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("The date is %s", out)*/

	//HaicamInit()

	//go hikvisionProbe()
	//go dahuaProbe()
	//discoverCameras()

	// hikvision: root/abmabmabm1
	// dahua: admin/abmabmabm1

	fmt.Println("start")
	/*
	streams, code := getCameraStreams("192.168.124.39", 80, "root", "abmabmabm1")
	b, err := json.Marshal(streams)
	if err != nil {
		log.Println("error:", err)
	}
	fmt.Println(code, string(b))
	*/
	setIPAddress("192.168.125.146", 80, "admin", "abmabmabm1", "192.168.125.146", "192.168.125.1")

	//pullEvents("192.168.124.39", 80, "root", "abmabmabm1")

	quit := make(chan bool)
	select {
	case <-quit:
		//exit
		//default:
		//conintue
	}

}
