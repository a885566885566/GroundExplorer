#include "ServerConnect.h"

void send_request(int data){
    WiFiClient client;
    if (!client.connect(hostname, port_number)) {  // HTTP
        ERROR("Connect Failed")
        return;
    }
    String url = "/data?esp_data=";
    url += data;

    client.print(String("GET ") + url + " HTTP/1.1\r\n" + 
            "Host: " + host + "\r\n" + 
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
}

void wifi_init(){
    WiFi.mode(WIFI_STA);
    WiFi.begin( AP_NAME, AP_PASSWD);
    while(WiFi.status() != WL_CONNECTED)
        delay(500);
}
