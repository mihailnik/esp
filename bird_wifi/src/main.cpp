#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>
#include <AsyncElegantOTA.h>

// i2s lib
//#include <SPIFFS.h>
#include "WAVFileReader.h"
#include "SinWaveGenerator.h"
#include "I2SOutput.h"
#define AmpSDn_pin 19
i2s_pin_config_t i2sPins = {
    .bck_io_num = GPIO_NUM_26,
    .ws_io_num = GPIO_NUM_25,
    .data_out_num = GPIO_NUM_33,
    .data_in_num = -1};

I2SOutput *output;
SampleSource *sampleSource;
//end i2s lib

// Replace with your network credentials
const char* ssid = "Kozaki_WiFi";
const char* password = "0676181665";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Set number of outputs
#define NUM_OUTPUTS  4

// Assign each GPIO to an output
int outputGPIOs[NUM_OUTPUTS] = {22, 4, 12, 14};

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

String getOutputStates(){
  JSONVar myArray;
  for (int i =0; i<NUM_OUTPUTS; i++){
    myArray["gpios"][i]["output"] = String(outputGPIOs[i]);
    myArray["gpios"][i]["state"] = String(digitalRead(outputGPIOs[i]));
  }
  String jsonString = JSON.stringify(myArray);
  return jsonString;
}

void notifyClients(String state) {
  ws.textAll(state);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "states") == 0) {
      notifyClients(getOutputStates());
    }
    else{
      int gpio = atoi((char*)data);
      digitalWrite(gpio, !digitalRead(gpio));
      notifyClients(getOutputStates());
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);


  // Set GPIOs as outputs
  for (int i =0; i<NUM_OUTPUTS; i++){
    pinMode(outputGPIOs[i], OUTPUT);
  }
  initSPIFFS();
  initWiFi();
  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html",false);
  });

  server.serveStatic("/", SPIFFS, "/");

  // Start ElegantOTA
  AsyncElegantOTA.begin(&server);
  
  // Start server
  server.begin();

// i2s lib
  pinMode(AmpSDn_pin, OUTPUT); // выключение усилителя/ включение (max98357a режим только левый канал)
  digitalWrite(AmpSDn_pin, HIGH); 
  Serial.println("Starting up");

  //SPIFFS.begin();

  Serial.println("Created sample source");

  // sampleSource = new SinWaveGenerator(40000, 10000, 0.75);

  // sampleSource = new WAVFileReader("/002.wav");

  // Serial.println("Starting I2S Output");
  // output = new I2SOutput();
  // output->start(I2S_NUM_1, i2sPins, sampleSource);
//end i2s lib
}

void loop() {
//  sampleSource = new SinWaveGenerator(40000, 10000, 0.75);

  sampleSource = new WAVFileReader("/002.wav");

  Serial.println("Starting I2S Output");
  output = new I2SOutput();
  output->start(I2S_NUM_1, i2sPins, sampleSource);
  int pin22_state = LOW;
  int pin22_state_old = LOW;
  while (true)
  {
  ws.cleanupClients();

    pin22_state = digitalRead(22);

    if (pin22_state != pin22_state_old )
    {
      pin22_state_old = pin22_state;
      if(pin22_state == HIGH)
       {
          i2s_stop(I2S_NUM_1);
          output->m_sample_generator=
       }
        else
       {
          i2s_start(I2S_NUM_1);

       }
    }
  }
}