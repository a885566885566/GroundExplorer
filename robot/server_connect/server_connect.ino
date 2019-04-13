#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#define trigPin D5
#define echoPin D6


void send_request(int data){
  WiFiClient client;
  const char * host = "10.42.0.1";
  const int port = 10418;
  Serial.print("[HTTP] begin...\n");
  if (!client.connect(host, port)) {  // HTTP
    Serial.println("Connect Failed");
    delay(1000);
    return;
  }
  String url = "/data?esp_data=";
  url += data;

  client.print(String("GET ") + url + " HTTP/1.1\r\n" + 
  "Host: " + host + "\r\n" + 
  "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while(client.available() == 0){
    if(millis()- timeout > 5000){
      Serial.println(">>>Client timeout");
      client.stop();
      return;
    }
  }
  /*while(client.connected() || client.available()){
    if(client.available()){
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
  }*/String line = client.readStringUntil('\n');
      Serial.println(line);
  client.stop();
}
void wifi_init(){
  WiFi.mode(WIFI_STA);
  WiFi.begin("e94066157-X550VQ", "bpbfXOHT");
  while(WiFi.status() != WL_CONNECTED)
    delay(500);
}
void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  wifi_init();
}
void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration*0.34/2;
  Serial.println(distance);
  send_request(distance);
  send_request(distance);
}
