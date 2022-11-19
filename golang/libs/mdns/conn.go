package mdns

/*
#cgo darwin,app CFLAGS: -I${SRCDIR}/../../
#cgo darwin,app LDFLAGS: -L${SRCDIR}/../../build -lhaicam-ifaddrs -lstdc++
#cgo android,app CFLAGS: -I${SRCDIR}/../../
#cgo android,app LDFLAGS: -L${SRCDIR}/../../build -lhaicam-ifaddrs -lstdc++ -llog -landroid

#cgo android,box CFLAGS: -I${SRCDIR}/../../
#cgo android,box LDFLAGS: -L${SRCDIR}/../../box_proj/build -lhaicam-ifaddrs -lstdc++ -llog -landroid
#cgo windows,box CFLAGS: -I${SRCDIR}/../../
#cgo windows,box LDFLAGS: -L${SRCDIR}/../../box_proj/build -lhaicam-ifaddrs -lstdc++
#cgo linux,box darwin,box CFLAGS: -I${SRCDIR}/../../
#cgo linux,box darwin,box LDFLAGS: -L${SRCDIR}/../../box_proj/build -lhaicam-ifaddrs -lstdc++
#cgo darwin,box CFLAGS: -I${SRCDIR}/../../
#cgo darwin,box LDFLAGS: -L${SRCDIR}/../../box_proj/build -lhaicam-ifaddrs -lstdc++
#include "ifaddrs.h"
*/
import "C"

import (
	"context"
	"encoding/binary"
	"encoding/json"
	"errors"
	"math/big"
	"net"
	"sync"
	"time"
	"unsafe"

	"github.com/pion/logging"
	"golang.org/x/net/dns/dnsmessage"
	"golang.org/x/net/ipv4"
)

var macAddrCounter uint64 = 0xBEEFED220200

func newMACAddress() net.HardwareAddr {
	b := make([]byte, 8)
	binary.BigEndian.PutUint64(b, macAddrCounter)
	macAddrCounter++
	return b[2:]
}

func getInterfaces() []net.Interface {
	interfaces := []net.Interface{}

	addrsJsonCStr := C.getInterfacesAndIPs()
	addrsJson := C.GoString(addrsJsonCStr)
	C.free(unsafe.Pointer(addrsJsonCStr))

	if len(addrsJson) > 2 { // for non android return "[]", fix android 11 net.Interfaces() permission issue
		var addrs []map[string]interface{}
		err := json.Unmarshal([]byte(addrsJson), &addrs)
		if err == nil {
			index := 1
			for _, addr := range addrs {
				if addr["isIPv4"].(float64) == 1 {
					iface := net.Interface{
						Index:        index,
						MTU:          1500,
						Name:         addr["interface"].(string),
						HardwareAddr: newMACAddress(),
						Flags:        net.FlagUp | net.FlagMulticast,
					}
					index++

					if err == nil {
						interfaces = append(interfaces, iface)
					}
				}
			}
		}
	}

	return interfaces
}

// Conn represents a mDNS Server
type Conn struct {
	mu  sync.RWMutex
	log logging.LeveledLogger

	socket  *ipv4.PacketConn
	dstAddr *net.UDPAddr

	queryInterval time.Duration
	localNames    []string
	queries       []query

	closed chan interface{}
}

type query struct {
	nameWithSuffix  string
	queryResultChan chan queryResult
}

type queryResult struct {
	answer dnsmessage.ResourceHeader
	addr   net.Addr
}

const (
	inboundBufferSize    = 512
	defaultQueryInterval = time.Second
	destinationAddress   = "224.0.0.251:5353"
	maxMessageRecords    = 3
	responseTTL          = 120
)

// Server establishes a mDNS connection over an existing conn
func Server(conn *ipv4.PacketConn, config *Config) (*Conn, error) {
	if config == nil {
		return nil, errNilConfig
	}

	ifaces := getInterfaces()
	var err error

	if len(ifaces) == 0 {
		ifaces, err = net.Interfaces()
		if err != nil {
			return nil, err
		}
	}

	joinErrCount := 0
	for i := range ifaces {
		if err = conn.JoinGroup(&ifaces[i], &net.UDPAddr{IP: net.IPv4(224, 0, 0, 251)}); err != nil {
			joinErrCount++
		}
	}
	if joinErrCount >= len(ifaces) {
		return nil, errJoiningMulticastGroup
	}

	dstAddr, err := net.ResolveUDPAddr("udp", destinationAddress)
	if err != nil {
		return nil, err
	}

	loggerFactory := config.LoggerFactory
	if loggerFactory == nil {
		loggerFactory = logging.NewDefaultLoggerFactory()
	}

	localNames := []string{}
	for _, l := range config.LocalNames {
		localNames = append(localNames, l+".")
	}

	c := &Conn{
		queryInterval: defaultQueryInterval,
		queries:       []query{},
		socket:        conn,
		dstAddr:       dstAddr,
		localNames:    localNames,
		log:           loggerFactory.NewLogger("mdns"),
		closed:        make(chan interface{}),
	}
	if config.QueryInterval != 0 {
		c.queryInterval = config.QueryInterval
	}

	go c.start()
	return c, nil
}

// Close closes the mDNS Conn
func (c *Conn) Close() error {
	select {
	case <-c.closed:
		return nil
	default:
	}

	if err := c.socket.Close(); err != nil {
		return err
	}

	<-c.closed
	return nil
}

// Query sends mDNS Queries for the following name until
// either the Context is canceled/expires or we get a result
func (c *Conn) Query(ctx context.Context, name string) (dnsmessage.ResourceHeader, net.Addr, error) {
	select {
	case <-c.closed:
		return dnsmessage.ResourceHeader{}, nil, errConnectionClosed
	default:
	}

	nameWithSuffix := name + "."

	queryChan := make(chan queryResult, 1)
	c.mu.Lock()
	c.queries = append(c.queries, query{nameWithSuffix, queryChan})
	ticker := time.NewTicker(c.queryInterval)
	c.mu.Unlock()

	defer ticker.Stop()

	c.sendQuestion(nameWithSuffix)
	for {
		select {
		case <-ticker.C:
			c.sendQuestion(nameWithSuffix)
		case <-c.closed:
			return dnsmessage.ResourceHeader{}, nil, errConnectionClosed
		case res := <-queryChan:
			return res.answer, res.addr, nil
		case <-ctx.Done():
			return dnsmessage.ResourceHeader{}, nil, errContextElapsed
		}
	}
}

func ipToBytes(ip net.IP) (out [4]byte) {
	rawIP := ip.To4()
	if rawIP == nil {
		return
	}

	ipInt := big.NewInt(0)
	ipInt.SetBytes(rawIP)
	copy(out[:], ipInt.Bytes())
	return
}

func interfaceForRemote(remote string) (net.IP, error) {
	conn, err := net.Dial("udp", remote)
	if err != nil {
		return nil, err
	}

	localAddr := conn.LocalAddr().(*net.UDPAddr)
	if err := conn.Close(); err != nil {
		return nil, err
	}

	return localAddr.IP, nil
}

func (c *Conn) sendQuestion(name string) {
	packedName, err := dnsmessage.NewName(name)
	if err != nil {
		c.log.Warnf("Failed to construct mDNS packet %v", err)
		return
	}

	msg := dnsmessage.Message{
		Header: dnsmessage.Header{},
		Questions: []dnsmessage.Question{
			{
				Type:  dnsmessage.TypeA,
				Class: dnsmessage.ClassINET,
				Name:  packedName,
			},
		},
	}

	rawQuery, err := msg.Pack()
	if err != nil {
		c.log.Warnf("Failed to construct mDNS packet %v", err)
		return
	}

	if _, err := c.socket.WriteTo(rawQuery, nil, c.dstAddr); err != nil {
		c.log.Warnf("Failed to send mDNS packet %v", err)
		return
	}
}

func (c *Conn) sendAnswer(name string, dst net.IP) {
	packedName, err := dnsmessage.NewName(name)
	if err != nil {
		c.log.Warnf("Failed to construct mDNS packet %v", err)
		return
	}

	msg := dnsmessage.Message{
		Header: dnsmessage.Header{
			Response:      true,
			Authoritative: true,
		},
		Answers: []dnsmessage.Resource{
			{
				Header: dnsmessage.ResourceHeader{
					Type:  dnsmessage.TypeA,
					Class: dnsmessage.ClassINET,
					Name:  packedName,
					TTL:   responseTTL,
				},
				Body: &dnsmessage.AResource{
					A: ipToBytes(dst),
				},
			},
		},
	}

	rawAnswer, err := msg.Pack()
	if err != nil {
		c.log.Warnf("Failed to construct mDNS packet %v", err)
		return
	}

	if _, err := c.socket.WriteTo(rawAnswer, nil, c.dstAddr); err != nil {
		c.log.Warnf("Failed to send mDNS packet %v", err)
		return
	}
}

func (c *Conn) start() { //nolint gocognit
	defer func() {
		c.mu.Lock()
		defer c.mu.Unlock()
		close(c.closed)
	}()

	b := make([]byte, inboundBufferSize)
	p := dnsmessage.Parser{}

	for {
		n, _, src, err := c.socket.ReadFrom(b)
		if err != nil {
			return
		}

		func() {
			c.mu.RLock()
			defer c.mu.RUnlock()

			if _, err := p.Start(b[:n]); err != nil {
				c.log.Warnf("Failed to parse mDNS packet %v", err)
				return
			}

			for i := 0; i <= maxMessageRecords; i++ {
				q, err := p.Question()
				if errors.Is(err, dnsmessage.ErrSectionDone) {
					break
				} else if err != nil {
					c.log.Warnf("Failed to parse mDNS packet %v", err)
					return
				}

				for _, localName := range c.localNames {
					if localName == q.Name.String() {
						localAddress, err := interfaceForRemote(src.String())
						if err != nil {
							c.log.Warnf("Failed to get local interface to communicate with %s: %v", src.String(), err)
							continue
						}

						c.sendAnswer(q.Name.String(), localAddress)
					}
				}
			}

			for i := 0; i <= maxMessageRecords; i++ {
				a, err := p.AnswerHeader()
				if errors.Is(err, dnsmessage.ErrSectionDone) {
					return
				}
				if err != nil {
					c.log.Warnf("Failed to parse mDNS packet %v", err)
					return
				}

				if a.Type != dnsmessage.TypeA && a.Type != dnsmessage.TypeAAAA {
					continue
				}

				for i := len(c.queries) - 1; i >= 0; i-- {
					if c.queries[i].nameWithSuffix == a.Name.String() {
						c.queries[i].queryResultChan <- queryResult{a, src}
						c.queries = append(c.queries[:i], c.queries[i+1:]...)
					}
				}
			}
		}()
	}
}
