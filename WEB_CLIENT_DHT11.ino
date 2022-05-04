// Load Wi-Fi library
#include <WiFi.h>
#include "DHT.h"

const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";
#define DHTTYPE DHT11 
#define DHTPIN 15
#define SensorPin 34

WiFiServer server(80);
String header;

DHT dht(DHTPIN, DHTTYPE);

float moisture(){
  float sensorValue = 0; 
  Serial.println(" ");
  for (int i = 0; i <= 100; i++) 
 { 
   float ans = analogRead(SensorPin);
   sensorValue = sensorValue + ans; 
   delay(1); 
 } 
 Serial.println(" ");
 sensorValue = sensorValue/100.0; 
 Serial.println(sensorValue);
 return sensorValue;
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));
  dht.begin();
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients
  

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        
        if (c == '\n') 
        {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            
            float h = dht.readHumidity();
            float t = dht.readTemperature();
            float f = dht.readTemperature(true);
            // Compute heat index in Fahrenheit (the default)
            float hif = dht.computeHeatIndex(f, h);
            // Compute heat index in Celsius (isFahreheit = false)
            float hic = dht.computeHeatIndex(t, h, false);
            float sensorValue = moisture();
            sensorValue = ((4096 - sensorValue)/4096)*100.0;
            delay(30);
            Serial.println("Moisture Sensor Value"); 
            Serial.println(sensorValue);
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            client.println("<p>Humidity: " + String(h) + "%" + " Temperature: " + String(t) + "C" "</p>");
            client.println("<p> Soil Moisture: " + String(sensorValue) + "</p>");
            client.println("</body></html>");
            client.println();
            // Break out of the while loop
            break;
          } 
          else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } 
        else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
    delay(1000);
  }
}
