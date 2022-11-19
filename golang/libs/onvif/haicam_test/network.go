package main

import (
	
	"fmt"
	"io/ioutil"
	"log"
	"strconv"

	"github.com/use-go/onvif/gosoap"
	"github.com/beevik/etree"
	"github.com/use-go/onvif"
	"github.com/use-go/onvif/device"
	"github.com/use-go/onvif/xsd"
	onv "github.com/use-go/onvif/xsd/onvif"
)

func setIPAddress(cameraIPv4 string, port int, username string, password string, ipv4 string, gateway string, usingDHCP bool) {
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

	b, _ := ioutil.ReadAll(res.Body)

	doc := etree.NewDocument()
	err = doc.ReadFromBytes(b)
	if err != nil {
		panic("err");
	}

	str, _ := doc.WriteToString()
	log.Printf("GetNetworkInterfacesResponse output %+v %s", res.StatusCode, gosoap.SoapMessage(str).StringIndent())

	interfaces := doc.Root().FindElements("./Body/GetNetworkInterfacesResponse/NetworkInterfaces")

	if interfaces == nil {
		panic("GetNetworkInterfacesResponse/NetworkInterfaces failed")
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

	log.Printf("Interface Toten: %s %s", token, mac)

	res, err = dev.CallMethod(device.GetNetworkDefaultGateway{})
	b, _ = ioutil.ReadAll(res.Body)
	log.Printf("GetNetworkDefaultGateway output %+v %s", res.StatusCode, gosoap.SoapMessage(b).StringIndent())


        if(!usingDHCP) {
            res, err = dev.CallMethod(device.SetNetworkDefaultGateway{IPv4Address: onv.IPv4Address(gateway)})
            b, _ = ioutil.ReadAll(res.Body)

            doc = etree.NewDocument()
            err = doc.ReadFromBytes(b)
            if err != nil {
                panic("err");
            }

            str, _ = doc.WriteToString()
            log.Printf("SetNetworkDefaultGatewayResponse output %+v %s", res.StatusCode, gosoap.SoapMessage(str).StringIndent())

            response := doc.Root().FindElement("./Body/SetNetworkDefaultGatewayResponse")
            if response == nil {
                panic("SetNetworkDefaultGatewayResponse failed")
            }
        }

	res, err = dev.CallMethod(device.SetNetworkInterfaces{
		InterfaceToken: onv.ReferenceToken(token),
		NetworkInterface: onv.NetworkInterfaceSetConfiguration{
			Enabled: true,
			MTU: 1500,
			IPv4: onv.IPv4NetworkInterfaceSetConfiguration{
				Enabled: true,
				Manual: onv.PrefixedIPv4Address{
					Address: onv.IPv4Address(ipv4),
					PrefixLength: 24,
				},
				DHCP: xsd.Boolean(usingDHCP),
			},
			IPv6: onv.IPv6NetworkInterfaceSetConfiguration{
				Enabled: false,
				AcceptRouterAdvert: false,
				Manual: onv.PrefixedIPv6Address{
					Address: onv.IPv6Address("::"),// Unspecified Address ::/128
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
		panic("err");
	}

	str, _ = doc.WriteToString()
	log.Printf("SetNetworkInterfacesResponse output %+v %s", res.StatusCode, gosoap.SoapMessage(str).StringIndent())

	setNetworkInterfacesResponse := doc.Root().FindElement("./Body/SetNetworkInterfacesResponse")
	if setNetworkInterfacesResponse == nil {
		panic("SetNetworkInterfacesResponse failed")
	}
	rebootNeeded := setNetworkInterfacesResponse.SelectElement("RebootNeeded").Text()
	log.Printf("rebootNeeded %s", rebootNeeded)

	if rebootNeeded == "true" {
	    res, err = dev.CallMethod(device.SystemReboot{})
	    b, _ = ioutil.ReadAll(res.Body)
	    log.Printf("SystemReboot output %+v %s", res.StatusCode, gosoap.SoapMessage(b).StringIndent())
	}
}

func main() {
	setIPAddress("172.20.20.2", 80, "admin", "abmabmabm1", "192.168.125.146", "172.20.20.254", true)

	//pullEvents("192.168.124.39", 80, "root", "abmabmabm1")

	quit := make(chan bool)
	select {
	case <-quit:
		//exit
		//default:
		//conintue
	}
}
