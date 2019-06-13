#include <WiFi.h>

const char* ssid = "yourNetworkName";
const char* password =  "yourNetworkPass";

const uint16_t port = 8090;
const char * host = "192.168.1.83";

void setup()
{

    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("...");
    }

    Serial.print("WiFi connected with IP: ");
    Serial.println(WiFi.localIP());

}

void loop()
{
    WiFiClient client;

    if (!client.connect(host, port)) {
        Serial.println("Connection to host failed");
        delay(1000);
        return;
    }

    Serial.println("Connected to server successful!");

    while(true){
        if(WiFi.status() != WL_CONNECTED){
            Serial.println("Wifi disconnected!");
        }
        else{
            client.print("Hello from ESP32!");
            delay(1000);
        }
    }
    Serial.println("Disconnecting...");
    client.stop();

    delay(10000);
}
