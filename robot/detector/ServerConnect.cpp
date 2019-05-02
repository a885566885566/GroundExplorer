#include "ServerConnect.h"

void NetServer::webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
      USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

      // send message to server when Connected
      webSocket.sendTXT("Connected");
    }
      break;
    case WStype_TEXT:
      USE_SERIAL.printf("[WSc] get text: %s\n", payload);

      // send message to server
      // webSocket.sendTXT("message here");
      break;
    case WStype_BIN:
      USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
  }
}

void NetServer::init(){
    WiFiMulti.addAP(AP_NAME, AP_PASSWD);
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }
    // server address, port and URL
    webSocket.begin(HOST_NAME, PORT_NUMBER, PATH);
    // event handler
    webSocket.onEvent(webSocketEvent);
}
void NetServer::send_request(int data){
  String d(data)
  webSocket.sendTXT(d);
}
/*
void NetServer::send_request(int data){
    WiFiClient client;
    if (!client.connect(host_name, port_number)) {  // HTTP
        ERROR("Connect Failed")
        return;
    }
    String url = "/data?esp_data=";
    url += data;

    client.print(String("GET ") + url + " HTTP/1.1\r\n" + 
            "Host: " + host_name + "\r\n" + 
            "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while(client.available() == 0){
        if(millis()- timeout > SERVER_TIMEOUT){
            ERROR(">>>Client timeout")
            client.stop();
            return;
        }
    }
    String line = client.readStringUntil('\n');
    Serial.println(line);
    client.stop();
}*/
