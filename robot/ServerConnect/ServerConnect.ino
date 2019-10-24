#include <WiFi.h>

const char* ssid = "e94066157-X550VQ";
const char* password = "bpbfXOHT";

const uint16_t port = 10418;
//const char * host = "192.168.1.83";
IPAddress server(10,42,0,1);

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

    

    Serial.println("Connected to server successful!");

    while(true){
        if(WiFi.status() != WL_CONNECTED){
            Serial.println("Wifi disconnected!");
        }
        else{
          if (!client.connect(server, port)) {
                Serial.println("Connection to host failed");
                delay(1000);
                return;
          }
          String msg = "GET /data?esp_data=";
          msg += millis();
            client.println(msg);
            client.println();
            //delay(1000);
        }
    }
    Serial.println("Disconnecting...");
    client.stop();

    delay(10000);
}
