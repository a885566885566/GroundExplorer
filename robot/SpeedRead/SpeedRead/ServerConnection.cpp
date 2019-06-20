#ifndef _SERVER_H
#define _SERVER_H
#include <WiFi.h>
#include "ServerConnection.h"
//#include "Core.h"

#define ssid "e94066157-X550VQ"
#define password "bpbfXOHT"

#define port 10418
#define host "10.42.0.1"
#define ERROR( msg ) Serial.println(msg);
#define LOG( msg ) Serial.println(msg);
/*
const char* ssid = "e94066157-X550VQ";
const char* password = "bpbfXOHT";

const uint16_t port = 10418;*/
//const char * host = "192.168.1.83";
//IPAddress server(10,42,0,1);

void wifi_init(){
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        LOG("...")
    }
    LOG("WiFi connected with IP: ");
    LOG(WiFi.localIP());
}

void send_data(String msg){
    WiFiClient client;
    if(WiFi.status() != WL_CONNECTED){
        ERROR("Wifi disconnected!");
    }
    else{
      IPAddress server(10,42,0,1);
        if (!client.connect(host, port)) {
        //if (!client.connect(server, port)) {
            ERROR("Connection to host failed");
            return;
        }
        String msg_buf = "GET /data?esp_data=";
        msg_buf += msg;
        client.println(msg_buf);
        client.println();
    }
}
#endif
