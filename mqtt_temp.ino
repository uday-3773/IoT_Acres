#include <WiFi.h>
#include <PubSubClient.h>

#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT22

// WiFi
const char* ssid = "Quadri";
const char* password = "summertime";

// MQTT Broker
const char *mqtt_broker = "172.20.10.5";
const int mqtt_port = 1883;
const char *topic1 = "IoT_Acres/Hum";
const char *topic2 = "IoT_Acres/Temp";

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(DHTPIN, DHTTYPE);

uint8_t data1[11];
uint8_t data2[11];
String deviceId = WiFi.macAddress();

void setup() {
 // Set software serial baud to 115200;
 Serial.begin(115200);
 
 //--------
  data1[0] = strtoul(deviceId.substring(0,2).c_str(), NULL, 16); 
  data1[1] = strtoul(deviceId.substring(3,5).c_str(), NULL, 16); 
  data1[2] = strtoul(deviceId.substring(6,8).c_str(), NULL, 16); 
  data1[3] = strtoul(deviceId.substring(9,11).c_str(), NULL, 16); 
  data1[4] = strtoul(deviceId.substring(12,14).c_str(), NULL, 16); 
  data1[5] = strtoul(deviceId.substring(15).c_str(), NULL, 16);
 
  data2[0] = strtoul(deviceId.substring(0,2).c_str(), NULL, 16); 
  data2[1] = strtoul(deviceId.substring(3,5).c_str(), NULL, 16); 
  data2[2] = strtoul(deviceId.substring(6,8).c_str(), NULL, 16); 
  data2[3] = strtoul(deviceId.substring(9,11).c_str(), NULL, 16); 
  data2[4] = strtoul(deviceId.substring(12,14).c_str(), NULL, 16); 
  data2[5] = strtoul(deviceId.substring(15).c_str(), NULL, 16);
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

  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);
  
  /*Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));*/

  //String h_out = String(h);

  //dtostrf(h,0,2,data1);
  //dtostrf(t,0,2,data2);

  if (sizeof(data1) - 6 >= sizeof(float)) {
    // Copy the float into the next 4 bytes of data1, starting at position 6
    memcpy(data1 + 6, &h, sizeof(float));
  } else {
    // Handle error: not enough space to store the float
    Serial.println("Error: Not enough space to store the float");
  }

  if (sizeof(data2) - 6 >= sizeof(float)) {
    // Copy the float into the next 4 bytes of data1, starting at position 6
    memcpy(data2 + 6, &t, sizeof(float));
  } else {
    // Handle error: not enough space to store the float
    Serial.println("Error: Not enough space to store the float");
  }

  for(int i = 0; i < sizeof(data1); i++){
      if(i == 0){
        Serial.print("Data 1: ");
        Serial.print(data1[i]);
      }else{
        Serial.print(data1[i]);
      }
  }
  Serial.println(' '); 

  for(int i = 0; i < sizeof(data2); i++){
      if(i == 0){
        Serial.print("Data 2: ");
        Serial.print(data2[i]);
      }else{
        Serial.print(data2[i]);
      }
  }
  Serial.println(' '); 

  /*client.publish(topic1, data1, false);
  client.publish(topic2, data2, false);*/
  client.publish(topic1, (const uint8_t *)data1, sizeof(data1));
  client.publish(topic2, (const uint8_t *)data2, sizeof(data2));


  //client.subscribe(topic);

  client.loop();

}
