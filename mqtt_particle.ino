#include <WiFi.h>
#include <PubSubClient.h>

#include "PMS.h"
#include <HardwareSerial.h>
#include <Wire.h>

PMS pms(Serial);
PMS::DATA data;

String val1;
String val2;
String val3;

// WiFi
const char* ssid = "Quadri";
const char* password = "summertime";

// MQTT Broker
const char *mqtt_broker = "172.20.10.5";
const int mqtt_port = 1883;
const char *topic = "IoT_Acres/MQTT";

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(DHTPIN, DHTTYPE);

void setup() {
 // Set software serial baud to 115200;
 Serial.begin(115200);
 // connecting to a WiFi network
 WiFi.begin(ssid, password);

 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 
 //client.setCallback(callback);
 
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str()))
     {
         Serial.println("Mqtt broker connected");
     } 
     else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
 // publish and subscribe
 //client.publish(topic, "Hi EMQX I'm ESP32 ^^");
 //client.subscribe(topic);

 Serial.println(F("DHTxx test!"));

 dht.begin();

}

/*void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
 }
 Serial.println();
 Serial.println("-----------------------");
}*/

void loop() {

   // Wait a few seconds between measurements.
  delay(2000);

  if (pms.read(data))
  {
    val1 = data.PM_AE_UG_1_0;
    val2 = data.PM_AE_UG_2_5;
    val3 = data.PM_AE_UG_10_0;
 
    Serial.println("Air Quality Monitor");
 
    Serial.println("PM1.0 :" + val1 + "(ug/m3)");
 
    Serial.println("PM2.5 :" + val2 + "(ug/m3)");
 
    Serial.println("PM10  :" + val3 + "(ug/m3)");


  //String h_out = String(h);
  char data1[10];
  char data2[10];
  char data3[10];

  dtostrf(val1,0,2,data1);
  dtostrf(val2,0,2,data2);
  dtostrf(val3,0,2,data3);

  client.publish(topic, data1, false);
  client.publish(topic, data2, false);
  client.publish(topic, data3, false);  

  //client.subscribe(topic);

  client.loop();

}
