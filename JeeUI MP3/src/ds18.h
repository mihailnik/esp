#include <OneWire.h>
#include <DallasTemperature.h>

#define DS_PIN 4

OneWire oneWire_ds(DS_PIN);
DallasTemperature ds_sensor(&oneWire_ds);

bool ds_setup;

float ds(int interval)
{
    

    static bool ds_setup;
    static unsigned long ds_timer;
    static unsigned int ds_interval = interval;
    static float t;

    if(!ds_setup)
    {
        ds_sensor.begin();
        ds_setup = true;
    }

    if (ds_timer + ds_interval > millis())
        return t;
    ds_timer = millis();
    ds_sensor.requestTemperatures();
    t = ds_sensor.getTempCByIndex(0);
}