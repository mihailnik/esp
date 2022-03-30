#include "Arduino.h"
#include "WiFi.h"
//#include <AsyncTCP.h>
#include <AsyncElegantOTA.h>
#include "ESPAsyncWebServer.h"
 
const char *ssid = "Korshun";
const char *password = "Gfhjkm.2013";
 
AsyncWebServer server(80);
 
void setup(){
  Serial.begin(115200);
 
  WiFi.softAP(ssid, password);
 
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
 
  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello World");
  });
 

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(){}