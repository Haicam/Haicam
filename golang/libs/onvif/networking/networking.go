package networking

import (
	"bytes"
	"net/http"
	"time"
)

// SendSoap send soap message
func SendSoap(endpoint, message string) (*http.Response, error) {
	//fmt.Println("Request[debug]: ", endpoint, gosoap.SoapMessage(message).StringIndent())
	//httpClient := new(http.Client)
	httpClient := &http.Client{
		Timeout: 10 * time.Second,
	}

	resp, err := httpClient.Post(endpoint, "application/soap+xml; charset=utf-8", bytes.NewBufferString(message))
	if err != nil {
		return resp, err
	}

	return resp, nil
}

// SendSoapWithTimeout send soap message with timeOut
func SendSoapWithTimeout(endpoint string, message []byte, timeout time.Duration) (*http.Response, error) {
	httpClient := &http.Client{
		Timeout: timeout,
	}

	return httpClient.Post(endpoint, "application/soap+xml; charset=utf-8", bytes.NewReader(message))
}