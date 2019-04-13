#ifndef SERVER_CONNECT_H
#define SERVER_CONNECT_H


#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define AP_NAME "e94066157-X550VQ"
#define AP_PASSWD "bpbfXOHT"

#define SERVER_TIMEOUT 5000

const char *hostname = "10.42.0.1";     // domain name is available
const int port_number = 10418;

void wifiInit();
void sendRequest();

#endif
