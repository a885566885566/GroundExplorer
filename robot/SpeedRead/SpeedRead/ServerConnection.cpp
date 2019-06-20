#ifndef _SERVER_H
#define _SERVER_H
#include <WiFi.h>
#include "ServerConnection.h"
#include "constants.h"

#define ssid "e94066157-X550VQ"
#define password "bpbfXOHT"
//#define ssid "NCKU250"
//#define password "e94066157"

#define port 10418
#define host "10.42.0.1"
//#define host "10.1.1.16"
#define ERROR( msg ) Serial.println(msg);
#define LOG( msg ) Serial.println(msg);
/*
const char* ssid = "e94066157-X550VQ";
const char* password = "bpbfXOHT";

const uint16_t port = 10418;*/
//const char * host = "192.168.1.83";
//IPAddress server(10,42,0,1);
WiFiClient client;
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
    if(WiFi.status() != WL_CONNECTED){
        ERROR("Wifi disconnected!");
    }
    else{
      //IPAddress server(10,42,0,1);
        if (!client.connect(host, port)) {
        //if (!client.connect(server, port)) {
            ERROR("Connection to host failed");
            return;
        }
        String msg_buf = "GET /data?esp_data=";
        msg_buf += msg;
        LOG(msg_buf)
        client.println(msg_buf);
        client.println();
    }
}

void send_update(double px, double py, double v, double th){
    LOG("enter");
    String msg = "?mode=" + String(MODE_UPDATE) + "&id="+ String(ROBOT_ID)+ "&px=" + px + "&py=" + py + "&v=" + v + "&th=" + th;
    if(WiFi.status() != WL_CONNECTED){
        ERROR("Wifi disconnected!");
    }
    else{
      //IPAddress server(10,42,0,1);
        LOG("connect");
        if (!client.connect(host, port)) {
        //if (!client.connect(server, port)) {
            ERROR("Connection to host failed");
            return;
        }
        String msg_buf = "GET /esp_data";
        msg_buf += msg;
        LOG(msg_buf);
        LOG("send");
        client.println(msg_buf);
        //client.println();
    }
    LOG("leave");
}
#endif
