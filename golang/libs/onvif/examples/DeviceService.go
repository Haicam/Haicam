package main

import (
	"fmt"
	"io/ioutil"
	"net/http"

	goonvif "github.com/use-go/onvif"
	"github.com/use-go/onvif/event"
	"github.com/use-go/onvif/gosoap"
)

const (
	login    = "admin"
	password = "123456"
)

func readResponse(resp *http.Response) string {
	b, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		panic(err)
	}
	return string(b)
}

func printResponse(resp *http.Response) {
	b, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		panic(err)
	}
	fmt.Println(gosoap.SoapMessage(string(b)).StringIndent())
}

func main1() {
	//Getting an camera instance
	dev, err := goonvif.NewDevice("192.168.125.137")
	if err != nil {
		panic(err)
	}
	//Authorization
	dev.Authenticate(login, password)

	//Preparing commands
	/*getCapabilities := device.GetCapabilities{Category: "All"}
	createUser := device.CreateUsers{User: onvif.User{
		Username:  "TestUser",
		Password:  "TestPassword",
		UserLevel: "User",
	},
	}*/

	//Commands execution
	//systemDateAndTyme := device.GetSystemDateAndTime{}
	//r, err := dev.CallMethod(systemDateAndTyme)
	//if err != nil {
	//	log.Println(err)
	//}
	//printResponse(r)

	fmt.Println("=======================")

	//p := event.CreatePullPointSubscription{InitialTerminationTime: "PT600S"}
	//res, err := dev.CallMethod(p)

	p := event.PullMessages{Timeout: "PT600S", MessageLimit: 500}
	res, err := dev.CallMethodDo("http://192.168.125.137:80/onvif/event/subsription_76", p)

	printResponse(res)
}
