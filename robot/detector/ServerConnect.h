#ifndef SERVER_CONNECT_H
#define SERVER_CONNECT_H

#define ERROR( msg ) Serial.println(msg);

#include "Core.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "arduinoWebSockets/src/WebSocketsClient.h"
#include <Hash.h>

#define AP_NAME "e94066157-X550VQ"
#define AP_PASSWD "bpbfXOHT"

#define HOST_NAME "10.42.0.1"     // domain name is available
#define PORT_NUMBER 10418
#define PATH "/"

#define USE_SERIAL Serial
#define SERVER_TIMEOUT 5000


class NetServer{
    private:
        void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
        ESP8266WiFiMulti WiFiMulti;
        WebSocketsClient webSocket;
    public:
        void init();
        void send_request(int data);
};

void wifi_init();
void send_request(int data);

#endif
