#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#else
#include <HTTPClient.h>
#endif

String utc_ti_local(String ts)
{
    long epoch = ts.toInt() + (param(F("tz")).toInt() * 3600);
    String t;
    Serial.print("The UTC time is ");     // UTC is the time at Greenwich Meridian (GMT)
    t += String((epoch % 86400L) / 3600); // print the hour (86400 equals secs per day)
    t += String(':');
    if (((epoch % 3600) / 60) < 10)
    {
        t += String('0');
    }
    t += String((epoch % 3600) / 60); // print the minute (3600 equals secs per minute)
    return t;
}

String openweathermap(float lat, float lon, String ApiKey)
{
    // 1d230c9a48414be04746d6d5b08a3919
    if (!get_ip)
        return "Нет подключения к сети";
    static String weather;
    static unsigned long weather_timer;        // таймер проверки событий
    static unsigned long interval_weather = 0; // интервал опроса событий 1 секунда

    if (weather_timer + interval_weather > millis())
        return weather;
    weather_timer = millis();

    String weatherString;
    HTTPClient http;
    http.begin(String(F("http://api.openweathermap.org/data/2.5/weather?lat=")) + String(lat,2) + "&lon=" + String(lon,2) + String(F("&APPID=")) + ApiKey + String(F("&lang=ru")));
    http.addHeader(F("Content-Type"), F("text/plain"));
    auto httpCode = http.GET();     //Close connection
    String line = http.getString(); //Get the response payload
    //http.end();                         //Close connection
    DEBG(String(F("CODE: ")) + String(httpCode)); //Print HTTP return code
    DEBG(line);

    if (httpCode != 200)
    {
        return weather;
    }
    line.replace('[', ' ');
    line.replace(']', ' ');
    DynamicJsonBuffer jsonBuf;
    JsonObject &root = jsonBuf.parseObject(line);
    String codeDescription = root[F("weather")][F("description")].as<String>();
    float temp = root[F("main")][F("temp")].as<float>();
    int humidity = root[F("main")][F("humidity")].as<int>();
    int pressure = root[F("main")][F("pressure")].as<int>();
    float windSpeed = root[F("wind")][F("speed")].as<float>();
    int windDeg = root[F("wind")][F("deg")].as<int>();
    String sunrise = root[F("sys")][F("sunrise")].as<String>();
    String sunset = root[F("sys")][F("sunset")].as<String>();
    int cod = root[F("cod")].as<int>();
    if (cod == 200)
        interval_weather = 7200000; // 7200000
    else
        interval_weather = 60000;
    temp = temp - 273; // Kelvin to Celcius
    windSpeed = windSpeed;
    pressure = pressure / 1.333;
    sunrise = utc_ti_local(sunrise);
    sunset = utc_ti_local(sunset);
    int sh = sunrise.indexOf(':');
    int si = 0;
    if (sunrise.indexOf("pm") > 0)
        si = 12;
    sunrise =
        String((sunrise.substring(0, sh).toInt() + si) / 10) +
        String((sunrise.substring(0, sh).toInt() + si) % 10) + ":" +
        String(sunrise.substring(sh + 1, 10).toInt() / 10) +
        String(sunrise.substring(sh + 1, 10).toInt() / 10);
    sh = sunset.indexOf(':');
    si = 0;
    if (sunset.indexOf("pm") > 0)
        si = 12;
    sunset =
        String((sunset.substring(0, sh).toInt() + si) / 10) +
        String((sunset.substring(0, sh).toInt() + si) % 10) + ":" +
        String(sunset.substring(sh + 1, 10).toInt() / 10) +
        String(sunset.substring(sh + 1, 10).toInt() / 10);

    DEBG(String(F("Data Description: ")) + codeDescription);
    DEBG(String(F("Data temp: ")) + String(temp));
    DEBG(String(F("Data humidity: ")) + String(humidity));
    DEBG(String(F("Data pressure: ")) + String(pressure));
    DEBG(String(F("Data windSpeed: ")) + String(windSpeed));
    DEBG(String(F("Data windDeg: ")) + String(windDeg));
    DEBG(String(F("Data sunrise: ")) + sunrise);
    DEBG(String(F("Data sunset: ")) + sunset);
    String windDegString;
    if (windDeg >= 345 || windDeg <= 22)
        windDegString = F("Северный");
    if (windDeg >= 23 && windDeg <= 68)
        windDegString = String(F("С-В"));
    if (windDeg >= 69 && windDeg <= 114)
        windDegString = F("Восточный");
    if (windDeg >= 115 && windDeg <= 160)
        windDegString = String(F("Ю-В"));
    if (windDeg >= 161 && windDeg <= 206)
        windDegString = F("Южный");
    if (windDeg >= 207 && windDeg <= 252)
        windDegString = String(F("Ю-З"));
    if (windDeg >= 253 && windDeg <= 298)
        windDegString = F("Западный");
    if (windDeg >= 299 && windDeg <= 344)
        windDegString = String(F("С-З"));
    weatherString = String(F("Сейчас ")) + codeDescription + " ";
    weatherString += String(temp, 0) + "\x1CС";
    weatherString += String(F(" Влажн ")) + String(humidity) + "% ";
    weatherString += String(F("Давл ")) + String(pressure) + String(F(" мм "));
    weatherString += String(F("Ветер ")) + windDegString + " " + String(windSpeed, 1) + String(F(" м/c"));
    //write_param(F("weather"), weatherString);
    weather = weatherString;
    DEBG(weather);
    int int_sunrise = (sunrise.substring(0, 2).toInt() * 60) + sunrise.substring(3, 5).toInt();
    int int_sunset = (sunset.substring(0, 2).toInt() * 60) + sunset.substring(3, 5).toInt();
    return weather;
}