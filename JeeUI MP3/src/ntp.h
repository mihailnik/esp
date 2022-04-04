
#include "time.h"
#include <TimeLib.h>
#include <NtpClientLib.h>

void processSyncEvent(NTPSyncEvent_t ntpEvent)
{
    if (ntpEvent)
    {
        DEBG("Ошибка NTP: ");
        if (ntpEvent == noResponse)
            DEBG("Сервер не ответил");
        else if (ntpEvent == invalidAddress)
            DEBG("Не верный адрес NTP сервера");
    }
    else
    {
        DEBG("Получено время NTP: ");
        DEBG(NTP.getTimeDateString(NTP.getLastNTPSync()));
    }
}

void ntp(int interval, String NTPhost, int8_t timeZone)
{
    static bool ntp_setup;
    if (!get_ip)
        return;
    if (!ntp_setup)
    {
        NTP.begin(NTPhost, timeZone, true, 0);
        NTP.setInterval(interval);
        ntp_setup = true;
    }
    DEBG(NTP.getTimeDateString() + " Включено: " + NTP.getUptimeString() + " Обновлено: " + NTP.getTimeDateString(NTP.getFirstSync()).c_str());
}

String convertMon(int n)
{
  switch (n)
  {
    case 1: return F("Января");
    case 2: return F("Февраля");
    case 3: return F("Марта");
    case 4: return F("Апреля");
    case 5: return F("Мая");
    case 6: return F("Июня");
    case 7: return F("Июля");
    case 8: return F("Августа");
    case 9: return F("Сентября");
    case 10: return F("Октября");
    case 11: return F("Ноября");
    case 12: return F("Декабря"); 
  }
}

String convertWeek(int n)
{
  switch (n)
  {
    case 2: return F("Понедельник");
    case 3: return F("Вторник");
    case 4: return F("Среда");
    case 5: return F("Четверг");
    case 6: return F("Пятница");
    case 7: return F("Суббота");
    case 1: return F("Воскресенье");
  }
}

String timeString()
{
    return String(hour() / 10) + String(hour() % 10) + ":" + String(minute() / 10) + String(minute() % 10);
}

String dateString()
{
    return convertWeek(weekday()) + " " + String(day()) + " " + convertMon(month()) + " " + String(year());
}