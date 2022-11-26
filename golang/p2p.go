package main

//gcc –static a.c -Wl,-Bstatic -lm -Wl,-Bdynamic -lc

/*
#cgo darwin,app LDFLAGS: -L${SRCDIR}/build -lhaicam-p2p -lstdc++
#cgo android,app LDFLAGS: -L${SRCDIR}/build -lhaicam-p2p -lstdc++ -llog -landroid
#cgo android,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lstdc++ -llog -landroid
#cgo linux,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lstdc++
#cgo darwin,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -lstdc++
#cgo windows,box LDFLAGS: -L${SRCDIR}/box_proj/build -lhaicam-p2p-onvif -static-libgcc -static-libstdc++ -static
#include "haicam/P2Peer.h"
*/
import "C"

import (
	"bytes"
	"compress/gzip"
	"encoding/hex"
	"errors"
	"fmt"
	"log"
	"strings"
	"sync"
	"time"
	"unsafe"

	//"encoding/base64"
	"io/ioutil"

	"encoding/json"

	"github.com/google/uuid"
	"github.com/pion/rtcp"
	"github.com/pion/webrtc/v3"
	"github.com/pion/webrtc/v3/pkg/media"
)

/*const (
	bufferedAmountLowThreshold uint64 = 1024 * 1024 * 20  // 30 MB
	maxBufferedAmount          uint64 = 1024 * 1024 * 100 // 100 MB
)*/

//https://developer.mozilla.org/en-US/docs/Web/API/RTCIceCandidate
//https://webrtc.github.io/samples/src/content/peerconnection/trickle-ice/
/*var servers []webrtc.ICEServer = []webrtc.ICEServer{
	{
		URLs:           []string{"turn:192.168.1.222:19001?transport=udp"},
		Username:       "u1serxxyy",
		Credential:     "p1asswd9732H",
		CredentialType: webrtc.ICECredentialTypePassword,
	},
	{
		URLs: []string{"stun:stun.l.google.com:19302"},
	},
	{
		URLs:           []string{"turn:52.65.128.52:19001?transport=udp"},
		Username:       "u1serxxyy",
		Credential:     "p1asswd9732H",
		CredentialType: webrtc.ICECredentialTypePassword,
	},
	{
		URLs:           []string{"turn:34.214.6.54:19001?transport=udp"},
		Username:       "u1serxxyy",
		Credential:     "p1asswd9732H",
		CredentialType: webrtc.ICECredentialTypePassword,
	},
}*/

func check(err error) {
	if err != nil {
		Golog("panic:", err.Error())
		panic(err)
	}
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

	m := C.CString(msg)
	C.onGoLog(m)
	C.free(unsafe.Pointer(m))
}

type Peer struct {
	uuid                       string
	cObjRef                    unsafe.Pointer
	conn                       *webrtc.PeerConnection
	dataChannel                *webrtc.DataChannel
	videoTrack                 *webrtc.TrackLocalStaticSample
	audioTrack                 *webrtc.TrackLocalStaticSample
	isReady                    bool
	pendingCandidatesForLocal  []*webrtc.ICECandidateInit
	pendingCandidatesForRemote []*webrtc.ICECandidate
	candidatesMux              sync.Mutex
	isOffer                    bool
	//sendMoreCh                 chan struct{}
	isClosed     bool
	keyFrameOnly bool
}

var peersHolder map[string]*Peer = make(map[string]*Peer)

func (peer *Peer) setRemoteDescription(remoteDesc string) {
	var desc webrtc.SessionDescription
	err := json.Unmarshal([]byte(remoteDesc), &desc)
	check(err)

	err = peer.conn.SetRemoteDescription(desc)
	check(err)

	peer.candidatesMux.Lock()
	defer peer.candidatesMux.Unlock()

	for _, c := range peer.pendingCandidatesForLocal {
		check(peer.conn.AddICECandidate(*c))
	}

	peer.pendingCandidatesForLocal = make([]*webrtc.ICECandidateInit, 0)

	for _, c := range peer.pendingCandidatesForRemote {
		str := C.CString(zipEncodeBase64(c.ToJSON().Candidate))
		C.onICECandidateForRemote(peer.cObjRef, str)
		C.free(unsafe.Pointer(str))
	}

	peer.pendingCandidatesForRemote = make([]*webrtc.ICECandidate, 0)
}

func (peer *Peer) addPendingCandidateForRemote(c *webrtc.ICECandidate) {
	peer.candidatesMux.Lock()
	defer peer.candidatesMux.Unlock()
	peer.pendingCandidatesForRemote = append(peer.pendingCandidatesForRemote, c)
}

func (peer *Peer) addPendingCandidateForLocal(c *webrtc.ICECandidateInit) {
	peer.candidatesMux.Lock()
	defer peer.candidatesMux.Unlock()
	peer.pendingCandidatesForLocal = append(peer.pendingCandidatesForLocal, c)
}

func (peer *Peer) addICECandidate(candidate string) {
	c := &webrtc.ICECandidateInit{Candidate: candidate}
	desc := peer.conn.RemoteDescription()
	if desc == nil {
		peer.addPendingCandidateForLocal(c)
	} else {
		check(peer.conn.AddICECandidate(*c))
	}
}

func (peer *Peer) sendData(data []byte) {
	if peer.dataChannel == nil {
		return
	}

	err := peer.dataChannel.Send(data)
	if err != nil {
		//Peer sendData: read/write on closed pipe
	}

	/*if peer.isOffer && (peer.dataChannel.BufferedAmount()+uint64(len(data)) > maxBufferedAmount) {
		Golog("Wait until the bufferedAmount becomes lower than the threshold")
		select {
		case <-peer.sendMoreCh:
		case <-time.After(60 * time.Second):
		}
		Golog("The bufferedAmount becomes lower than the threshold")
	}*/
}

func (peer *Peer) sendVideoSample(data []byte, millisecond int, isKeyFrame int) {
	if peer.videoTrack == nil {
		return
	}

	if peer.keyFrameOnly && isKeyFrame == 0 {
		return
	}

	err := peer.videoTrack.WriteSample(media.Sample{Data: data, Duration: time.Millisecond * time.Duration(millisecond)})
	if err != nil {
		//Peer sendData: read/write on closed pipe ?
	}
}

func (peer *Peer) sendAudioSample(data []byte, millisecond int) {
	if peer.audioTrack == nil {
		return
	}
	err := peer.audioTrack.WriteSample(media.Sample{Data: data, Duration: time.Millisecond * time.Duration(millisecond)})
	check(err)
}

func (peer *Peer) close() {
	if peer.dataChannel != nil {
		err := peer.dataChannel.Close()
		if err != nil {
			log.Println(err)
		}
	}

	if peer.conn != nil {
		err := peer.conn.Close()
		if err != nil {
			log.Println(err)
		}
	}
}

//export SetRemoteDescription
func SetRemoteDescription(goObjRef C.ulonglong, remoteDesc *C.char) {
	//log.Printf("SetRemoteDescription goObjRef %d\n", uintptr(goObjRef))
	peer := (*Peer)(unsafe.Pointer(uintptr(goObjRef)))
	peer.setRemoteDescription(base64DecodeUnzip(C.GoString(remoteDesc)))
}

//export AddICECandidate
func AddICECandidate(goObjRef C.ulonglong, candidate *C.char) {
	//log.Printf("AddICECandidate goObjRef %d\n", uintptr(goObjRef))
	peer := (*Peer)(unsafe.Pointer(uintptr(goObjRef)))
	peer.addICECandidate(base64DecodeUnzip(C.GoString(candidate)))
}

//export SendData
func SendData(goObjRef C.ulonglong, data unsafe.Pointer, length C.int) {
	//log.Printf("SendData  goObjRef %d\n", uintptr(goObjRef))
	//slice:= (*[1 << 28]C.char)(data)[:length:length]
	slice := C.GoBytes(data, length) // copy
	peer := (*Peer)(unsafe.Pointer(uintptr(goObjRef)))
	peer.sendData(slice)
}

//export SendVideoSample
func SendVideoSample(goObjRef C.ulonglong, data unsafe.Pointer, length C.int, duration C.int, isKeyFrame C.int) {
	//log.Printf("SendData  goObjRef %d\n", uintptr(goObjRef))
	//slice:= (*[1 << 28]C.char)(data)[:length:length]
	slice := C.GoBytes(data, length) // copy
	peer := (*Peer)(unsafe.Pointer(uintptr(goObjRef)))
	peer.sendVideoSample(slice, int(duration), int(isKeyFrame))
}

//export SendAudioSample
func SendAudioSample(goObjRef C.ulonglong, data unsafe.Pointer, length C.int, duration C.int) {
	//log.Printf("SendData  goObjRef %d\n", uintptr(goObjRef))
	//slice:= (*[1 << 28]C.char)(data)[:length:length]
	slice := C.GoBytes(data, length) // copy
	peer := (*Peer)(unsafe.Pointer(uintptr(goObjRef)))
	peer.sendAudioSample(slice, int(duration))
}

//export ClosePeer
func ClosePeer(goObjRef C.ulonglong) {
	Golog("ClosePeer  goObjRef %d\n", uintptr(goObjRef))
	peer := (*Peer)(unsafe.Pointer(uintptr(goObjRef)))
	peer.isClosed = true
	peer.close()
	delete(peersHolder, peer.uuid)
}

//export CreateOfferer
func CreateOfferer(cObjRefPtr unsafe.Pointer, iceServersJson *C.char) C.ulonglong {
	var peer Peer
	peer.uuid = uuid.New().String()
	peer.cObjRef = cObjRefPtr
	peer.isReady = false
	peer.isOffer = true
	peer.pendingCandidatesForRemote = make([]*webrtc.ICECandidate, 0)
	peer.pendingCandidatesForLocal = make([]*webrtc.ICECandidateInit, 0)
	peer.isClosed = false

	iceServersJsonStr := C.GoString(iceServersJson)

	peersHolder[peer.uuid] = &peer

	go func() {

		var iceServers []webrtc.ICEServer
		json.Unmarshal([]byte(iceServersJsonStr), &iceServers)

		config := webrtc.Configuration{
			ICEServers: iceServers,
		}

		pc, err := webrtc.NewPeerConnection(config)
		check(err)

		peer.conn = pc

		pc.OnICEConnectionStateChange(func(cs webrtc.ICEConnectionState) {
			if cs == webrtc.ICEConnectionStateDisconnected || cs == webrtc.ICEConnectionStateFailed {
				Golog("EVC:webrtc.ICEConnectionStateDisconnected or failed")
				peer.isReady = false
				if !peer.isClosed {
					peer.isClosed = true
					C.onClose(peer.cObjRef)
				}
			}
		})

		// Take note that the handler is gonna be called with a nil pointer when gathering is finished.
		pc.OnICECandidate(func(c *webrtc.ICECandidate) {
			if c == nil {
				return
			}

			desc := peer.conn.RemoteDescription()
			if desc == nil {
				peer.addPendingCandidateForRemote(c)
			} else {
				str := C.CString(zipEncodeBase64(c.ToJSON().Candidate))
				C.onICECandidateForRemote(peer.cObjRef, str)
				C.free(unsafe.Pointer(str))
			}
		})

		ordered := true

		options := &webrtc.DataChannelInit{
			Ordered: &ordered,
		}

		//peer.sendMoreCh = make(chan struct{})

		// Create a datachannel with label 'data'
		dc, err := pc.CreateDataChannel("data", options)
		check(err)

		peer.dataChannel = dc

		// Register channel opening handling
		dc.OnOpen(func() {
			peer.isReady = true
			C.onReady(peer.cObjRef)
		})

		dc.OnError(func(err error) {
			Golog("EVC:webrtc err")
			panic(err)
		})

		dc.OnClose(func() {
			Golog("EVC:webrtc.OnClose")
			peer.isReady = false
			if !peer.isClosed {
				peer.isClosed = true
				C.onClose(peer.cObjRef)
			}
		})

		// Register the OnMessage to handle incoming messages
		dc.OnMessage(func(dcMsg webrtc.DataChannelMessage) {
			buf := dcMsg.Data
			C.onData(peer.cObjRef, unsafe.Pointer(&buf[0]), C.int(len(buf)))
		})

		/*
			// Set bufferedAmountLowThreshold so that we can get notified when
			// we can send more
			dc.SetBufferedAmountLowThreshold(bufferedAmountLowThreshold)

			// This callback is made when the current bufferedAmount becomes lower than the threadshold
			dc.OnBufferedAmountLow(func() {
				select {
				case peer.sendMoreCh <- struct{}{}:
				case <-time.After(120 * time.Second):
				}
			})*/

		// Now, create an offer
		offer, err := pc.CreateOffer(nil)
		check(err)

		// Create channel that is blocked until ICE Gathering is complete
		gatherComplete := webrtc.GatheringCompletePromise(pc)

		check(pc.SetLocalDescription(offer))
		//desc, err := json.Marshal(offer)
		//check(err)

		// Block until ICE Gathering is complete, disabling trickle ICE
		// we do this because we only can exchange one signaling message
		// in a production application you should exchange ICE Candidates via OnICECandidate

		select {
		case <-gatherComplete:
		case <-time.After(120 * time.Second):
		}

		offerDesc, err := json.Marshal(*pc.LocalDescription())
		check(err)

		str := C.CString(zipEncodeBase64(string(offerDesc)))
		C.onDescForRemote(peer.cObjRef, str)
		C.free(unsafe.Pointer(str))
	}()

	//log.Printf("CreateOfferer  %d\n", uintptr(unsafe.Pointer(&peer)))

	return C.ulonglong(uintptr(unsafe.Pointer(&peer)))
}

func rtcpUnmarshal(rawData []byte) (packet rtcp.Packet, bytesprocessed int, err error) {
	var h rtcp.Header

	err = h.Unmarshal(rawData)
	if err != nil {
		return nil, 0, err
	}

	bytesprocessed = int(h.Length+1) * 4
	if bytesprocessed > len(rawData) {
		return nil, 0, errors.New("errPacketTooShort")
	}

	switch h.Type {
	case rtcp.TypeSenderReport:
		packet = new(rtcp.SenderReport)

	case rtcp.TypeReceiverReport:
		packet = new(rtcp.ReceiverReport)

	case rtcp.TypeSourceDescription:
		packet = new(rtcp.SourceDescription)

	case rtcp.TypeGoodbye:
		packet = new(rtcp.Goodbye)

	case rtcp.TypeTransportSpecificFeedback:
		switch h.Count {
		case rtcp.FormatTLN:
			packet = new(rtcp.TransportLayerNack)
		case rtcp.FormatRRR:
			packet = new(rtcp.RapidResynchronizationRequest)
		case rtcp.FormatTCC:
			packet = new(rtcp.TransportLayerCC)
		default:
			packet = new(rtcp.RawPacket)
		}

	case rtcp.TypePayloadSpecificFeedback:
		switch h.Count {
		case rtcp.FormatPLI:
			packet = new(rtcp.PictureLossIndication)
		case rtcp.FormatSLI:
			packet = new(rtcp.SliceLossIndication)
		case rtcp.FormatREMB:
			packet = new(rtcp.ReceiverEstimatedMaximumBitrate)
		case rtcp.FormatFIR:
			packet = new(rtcp.FullIntraRequest)
		default:
			packet = new(rtcp.RawPacket)
		}

	default:
		packet = new(rtcp.RawPacket)
	}

	return packet, bytesprocessed, nil
}

//export CreateAnswerer
func CreateAnswerer(cObjRefPtr unsafe.Pointer, offerDescStr *C.char, iceServersJson *C.char) C.ulonglong {
	var peer Peer
	peer.uuid = uuid.New().String()
	peer.cObjRef = cObjRefPtr
	peer.isReady = false
	peer.isOffer = false
	peer.pendingCandidatesForRemote = make([]*webrtc.ICECandidate, 0)
	peer.pendingCandidatesForLocal = make([]*webrtc.ICECandidateInit, 0)
	peer.isClosed = false
	peer.keyFrameOnly = false

	Golog("EVC11 RTCP :CreateAnswerer: %s", peer.uuid)

	peersHolder[peer.uuid] = &peer

	offerDesc := C.GoString(offerDescStr)
	iceServersJsonStr := C.GoString(iceServersJson)

	go func() {

		//https://www.cnblogs.com/chyingp/p/sdp-in-webrtc.html
		// o=alice 2890844526 2890844526 IN IP4 host.anywhere.com
		// o=<username> <sess-id> <sess-version> <nettype> <addrtype> <unicast-address>
		// s=<session name>
		//m=application 59110 UDP/DTLS/SCTP webrtc-datachannel
		//m=video 36607 UDP/TLS/RTP/SAVPF 96 97 98 99 100 101 102 121 127 120 125 107 108 109 35 36 124 119 123
		//m=audio 45029 UDP/TLS/RTP/SAVPF 111 103 104 9 0 8 106 105 13 110 112 113 126

		//Golog("webrtc.hasVideo: ", offerDesc)

		hasVideo := strings.Contains(offerDesc, "m=video")
		//hasAudio := strings.Contains(offerDesc, "m=audio")

		var iceServers []webrtc.ICEServer
		json.Unmarshal([]byte(iceServersJsonStr), &iceServers)

		// Prepare the configuration
		config := webrtc.Configuration{
			ICEServers: iceServers,
		}

		// Create a new PeerConnection
		pc, err := webrtc.NewPeerConnection(config)
		check(err)

		peer.conn = pc

		if hasVideo {
			videoTrack, videoTrackErr := webrtc.NewTrackLocalStaticSample(webrtc.RTPCodecCapability{MimeType: webrtc.MimeTypeH264}, "video", "pion")
			if videoTrackErr != nil {
				check(videoTrackErr)
			}
			rtpSender, videoTrackErr := pc.AddTrack(videoTrack)
			if videoTrackErr != nil {
				check(videoTrackErr)
			}

			peer.videoTrack = videoTrack

			// Read incoming RTCP packets
			// Before these packets are returned they are processed by interceptors. For things
			// like NACK this needs to be called.
			go func() {
				rtcpBuf := make([]byte, 1500)
				for {
					if _, _, rtcpErr := rtpSender.Read(rtcpBuf); rtcpErr != nil {
						Golog("RTCP ERR: ", rtcpErr.Error())
						return
					}

					pkt, _, err := rtcpUnmarshal(rtcpBuf)
					if err != nil {
						Golog("RTCP Unmarshal ERR: ", err.Error(), hex.EncodeToString(rtcpBuf))
					}

					switch pkt.(type) {
					case *rtcp.ReceiverReport:
						//Golog("RTCP ReceiverReport")
					case *rtcp.ReceiverEstimatedMaximumBitrate:
						Golog("EVC11 RTCP ReceiverEstimatedMaximumBitrate")
					// Usually, an RTCP NACK packet is used as retransmission request for lost packets
					case *rtcp.TransportLayerNack:
						Golog("EVC11 RTCP TransportLayerNack")
						peer.keyFrameOnly = true
					case *rtcp.RapidResynchronizationRequest:
						Golog("EVC11 RTCP RapidResynchronizationRequest")
					case *rtcp.Goodbye:
						Golog("EVC11 RTCP Goodbye")
					default:
						//Golog("RTCP")
					}
				}
			}()
		}

		pc.OnConnectionStateChange(func(cs webrtc.PeerConnectionState) {
			Golog("EVC11 RTCP :pc.OnConnectionStateChange: %v, %s", cs, peer.uuid)
			// most are same as OnICEConnectionStateChange in the same time
			if cs == webrtc.PeerConnectionStateFailed {
				Golog("EVC11 RTCP :pc close")
				peer.isReady = false
				if !peer.isClosed {
					peer.isClosed = true
					C.onClose(peer.cObjRef)
				}
			} else if cs == webrtc.PeerConnectionStateConnected {
				if hasVideo { // or just when data channel open
					peer.isReady = true
					C.onReady(peer.cObjRef)
				}
			}
		})

		pc.OnICEConnectionStateChange(func(cs webrtc.ICEConnectionState) {
			Golog("EVC11 RTCP :pc.OnICEConnectionStateChange: %v, %s", cs, peer.uuid)
			// first disconnect, then failed; or failed directly, after disconnect, it can connect again
			// but failed status will take longer time (25s) than disconnect, so we need to handle both
			//if cs == webrtc.ICEConnectionStateDisconnected || cs == webrtc.ICEConnectionStateFailed {}

		})

		pc.OnICECandidate(func(c *webrtc.ICECandidate) {
			if c == nil {
				return
			}

			desc := peer.conn.RemoteDescription()
			if desc == nil {
				peer.addPendingCandidateForRemote(c)
			} else {
				str := C.CString(zipEncodeBase64(c.ToJSON().Candidate))
				C.onICECandidateForRemote(peer.cObjRef, str)
				C.free(unsafe.Pointer(str))
			}
		})

		pc.OnDataChannel(func(dc *webrtc.DataChannel) {
			peer.dataChannel = dc

			// Register channel opening handling
			dc.OnOpen(func() {
				peer.isReady = true
				C.onReady(peer.cObjRef)
			})

			dc.OnError(func(err error) {
				Golog("EVC:webrtc err")
				panic(err)
			})

			dc.OnClose(func() {
				peer.isReady = false
				Golog("EVC:webrtc.OnClose")
				if !peer.isClosed {
					peer.isClosed = true
					C.onClose(peer.cObjRef)
				}
			})

			// Register the OnMessage to handle incoming messages
			dc.OnMessage(func(dcMsg webrtc.DataChannelMessage) {
				buf := dcMsg.Data
				C.onData(peer.cObjRef, unsafe.Pointer(&buf[0]), C.int(len(buf)))
			})
		})

		peer.setRemoteDescription(offerDesc)

		answer, err := pc.CreateAnswer(nil)
		check(err)

		// Create channel that is blocked until ICE Gathering is complete
		gatherComplete := webrtc.GatheringCompletePromise(pc)

		check(pc.SetLocalDescription(answer))

		// Block until ICE Gathering is complete, disabling trickle ICE
		// we do this because we only can exchange one signaling message
		// in a production application you should exchange ICE Candidates via OnICECandidate

		select {
		case <-gatherComplete:
		case <-time.After(120 * time.Second):
		}

		answerDesc, err := json.Marshal(*pc.LocalDescription())
		check(err)

		str := C.CString(zipEncodeBase64(string(answerDesc)))
		C.onDescForRemote(peer.cObjRef, str)
		C.free(unsafe.Pointer(str))
	}()

	//log.Printf("CreateAnswerer  %d\n", uintptr(unsafe.Pointer(&peer)))

	return C.ulonglong(uintptr(unsafe.Pointer(&peer)))
}

func zipEncodeBase64(in string) string {
	//b := zip([]byte(in))
	//return string(base64.StdEncoding.EncodeToString(b))
	return in
}

func base64DecodeUnzip(in string) string {
	/*b, err := base64.StdEncoding.DecodeString(in)
	if err != nil {
		panic(err)
	}
	return string(unzip(b))*/
	return in
}

func zip(in []byte) []byte {
	var b bytes.Buffer
	gz := gzip.NewWriter(&b)
	_, err := gz.Write(in)
	if err != nil {
		panic(err)
	}
	err = gz.Flush()
	if err != nil {
		panic(err)
	}
	err = gz.Close()
	if err != nil {
		panic(err)
	}
	return b.Bytes()
}

func unzip(in []byte) []byte {
	var b bytes.Buffer
	_, err := b.Write(in)
	if err != nil {
		panic(err)
	}
	r, err := gzip.NewReader(&b)
	if err != nil {
		panic(err)
	}
	res, err := ioutil.ReadAll(r)
	if err != nil {
		panic(err)
	}
	return res
}

func main() {}
