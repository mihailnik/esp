#include <Arduino.h>

//#define RELEASE   // Раскоментировать перед выпуском релиза. Это отключит вывод в серийный порт!
#define BUTTON 0    // Пин кнопки управления
#define LED 2       // Пин светодиода индикации
#define LED_INVERT  // Если нужно инвертировать светодиод

void mp3_setup(String mp3_file);

#include "html.h"
//#include "max7219.h"        // Драйвер дисплея
//#include "ds18.h"           // Драйвер DS18B20
//#include "ntp.h"            // Драйвер NTP (время из интернета)
//#include "OpenWeatherMap.h" // Драйвер погоды OpenWeatherMap
//#include "mqtt.h"           // Драйвер MQTT
#include "mp3.h"

void update(int interval);

void setup() {
    framework();
}

void loop() {
    framework_handle();
    update(10000);

    mp3_play();
}

void update(int interval)
{
    static unsigned long timer;
    static unsigned int interval_ = interval;

    if(timer + interval_ > millis()) // Если интервал еще не истек, или идет нанимация
        return;
    timer = millis();

}