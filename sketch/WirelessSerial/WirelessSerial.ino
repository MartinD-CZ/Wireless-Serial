/*======================================================================================
 * WirelessSerial - Simple sketch for using the esp8266 as a UART-to-WiFi bridge
 * Written by Martin Danek, 2018
 * 
 * Licensed under CC BY-NC-SA 2.5, https://creativecommons.org/licenses/by-nc-sa/2.5/
 * Basically feel free to use/modify/publish it as long as it is not commercial and
 * you link the original author.
 * Loosely based on the WiFiTelnetToSerial example from ESP8266 example folder.
 * 
 * See <> for more details.
======================================================================================= */

#include <ESP8266WiFi.h>

//======================================================
//IMPORTANT PART - MODIFY HERE
const char* ssid = "put-your-SSID-here";
const char* pass = "put-the-password-here";
IPAddress ip(192, 168, 2, 200);         //select something out of the DHCP range
IPAddress gateway(192, 168, 2, 1);      //called "default gateway" in ipconfig
IPAddress subnet(255, 255, 255, 0);     //called "subnet mask" in ipconfig
//======================================================
int baudrate = 0;
String buf = "";

WiFiServer server(23);
WiFiClient client;

void setup() {
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(0, HIGH);
  digitalWrite(2, HIGH);

  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    //blink as long as the WiFi is not connected
    digitalWrite(2, LOW);
    delay(250);
    digitalWrite(2, HIGH);
    delay(250);
  }

  digitalWrite(2, LOW);

  //start the Telnet server
  server.begin();
  server.setNoDelay(true);

  while (!client.connected())
  {
    digitalWrite(0, LOW);
    delay(250);
    digitalWrite(0, HIGH);
    delay(250);
    
    if (server.hasClient())
    {
       client = server.available();
       client.println("//Connected to terminal; input baud rate:");
       
       while (!client.available());
       buf = client.readString();
       baudrate = buf.toInt();
       client.print("//Setting baud rate to ");
       client.println(baudrate);
    }
  }

  digitalWrite(0, LOW);

  Serial.begin(baudrate);
  client.print("//WiFi signal strength: ");
  client.print(WiFi.RSSI());
  client.println(" dBm");
  client.println("//Device ready!");
  client.println("//=============");
}

void loop() {
  //checking for errors
  if (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(2, HIGH);
  }
  
  if (!client.connected())
  {
    digitalWrite(0, HIGH);
  }

  if (WiFi.RSSI() < -85)
  {
    client.println("//Low WiFi Signal Strenght!");
  }
  
  
  //check for data from the computer
  if (client && client.connected()){
    if(client.available()){
      while(client.available()) Serial.write(client.read());
    }
  }

    
  //check for data from the Serial
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    //push UART data to all connected telnet clients
      if (client && client.connected()){
        client.write(sbuf, len);
        //delay(1);
      }
  }
}
