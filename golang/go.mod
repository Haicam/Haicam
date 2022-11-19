module haicamp2p

go 1.14

require (
	github.com/StackExchange/wmi v1.2.1 // indirect
	github.com/beevik/etree v1.1.0
	github.com/gofrs/uuid v3.3.0+incompatible
	github.com/google/uuid v1.3.0
	github.com/gorilla/websocket v1.4.2
	github.com/libp2p/go-reuseport v0.1.0
	github.com/mattn/go-sqlite3 v1.14.5
	github.com/pion/randutil v0.1.0
	github.com/pion/rtcp v1.2.6
	github.com/pion/webrtc/v3 v3.1.0-beta.7
	github.com/shirou/gopsutil v3.21.9+incompatible
	github.com/use-go/onvif v0.0.0-20200817103923-4e696ec65aa9
	go.bug.st/serial v1.3.3
)

replace github.com/use-go/onvif => ./libs/onvif

replace github.com/pion/transport => ./libs/transport

replace github.com/pion/mdns => ./libs/mdns
