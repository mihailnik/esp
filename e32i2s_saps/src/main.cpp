#include <Arduino.h>

// i2s lib
#include <SPIFFS.h>
#include "WAVFileReader.h"
#include "SinWaveGenerator.h"
#include "I2SOutput.h"
//end i2s lib

// i2s lib
i2s_pin_config_t i2sPins = {
    .bck_io_num = GPIO_NUM_26,
    .ws_io_num = GPIO_NUM_25,
    .data_out_num = GPIO_NUM_33,
    .data_in_num = -1};

I2SOutput *output;
SampleSource *sampleSource;
//end i2s lib
void setup()
{
  Serial.begin(115200);

// i2s lib
  Serial.println("Starting up");

  SPIFFS.begin();

  Serial.println("Created sample source");

  // sampleSource = new SinWaveGenerator(40000, 10000, 0.75);

  sampleSource = new WAVFileReader("/sample.wav");

  Serial.println("Starting I2S Output");
  output = new I2SOutput();
  output->start(I2S_NUM_1, i2sPins, sampleSource);
//end i2s lib
}

void loop()
{
  
  // nothing to do here - everything is taken care of by tasks
}