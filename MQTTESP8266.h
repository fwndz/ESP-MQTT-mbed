
#if !defined(MQTTESP8266_H)
#define MQTTESP8266_H

#include "MQTTmbed.h"
#include "ESP8266Interface.h"
#include "MQTTSocket.h"

// This struct is only used to workaround the order that the interfaces are initialized
// MQTTSocket contains a TCPSocketConnection which needs the ESP8266Interface to be 
// instantiated first. Unfortunately the only way to instantiate a member before a superclass 
// is through another superclass.
struct MQTTESP8266Holder {
    MQTTESP8266Holder(PinName tx, PinName rx, PinName reset, const char *ssid, const char *pass) :
            _wifi(tx, rx, reset, ssid, pass) {}
    
    ESP8266Interface _wifi;
};

// Straightforward implementation of a MQTT interface
class MQTTESP8266 : public MQTTESP8266Holder, public MQTTSocket {    
private:
    MQTTESP8266Holder::_wifi;
    //ESP8266Interface _wifi;
    
public:    
    MQTTESP8266(PinName tx, PinName rx, PinName reset, const char *ssid, const char *pass) :
            MQTTESP8266Holder(tx, rx, reset, ssid, pass) {
        _wifi.init();
        _wifi.connect();
    }
    
    ESP8266Interface& getInterface() {
        return _wifi;
    }
    
    void reconnect() {
        _wifi.disconnect();
        _wifi.connect();
    }
};


#endif
