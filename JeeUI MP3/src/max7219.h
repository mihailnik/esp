#define FONTS_MAX7219
#define MAX_DEVICES 4 // количество матриц
#define CLK_PIN 14    // D5
#define DATA_PIN 13   // D7
#define CS_PIN 15     // D8

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#include "font_5x7.h" // подключаем шрифт
#include "sysUTF8.h"  // подключаем конвертор UTF8

bool max_setup;

int max_speed = 40;
char max_buf[256];
char max_t_buf[10];
int rnd;

bool max_animate;
String max_screen[20];
bool max_screen_scroll[20];
int max_current_screen;

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

typedef struct
{
  textEffect_t effect; // text effect to display
  char *psz;           // text string nul terminated
  uint16_t speed;      // speed multiplier of library default
  uint16_t pause;      // pause multiplier for library default
} sCatalog;

sCatalog catalog[] =
    {
        {PA_SLICE, "SLICE", 2, 1},
        {PA_MESH, "MESH", 10, 1},
        {PA_FADE, "FADE", 20, 1},
        {PA_WIPE, "WIPE", 5, 1},
        {PA_WIPE_CURSOR, "WPE_C", 4, 1},
        {PA_OPENING, "OPEN", 3, 1},
        {PA_OPENING_CURSOR, "OPN_C", 4, 1},
        {PA_CLOSING, "CLOSE", 3, 1},
        {PA_CLOSING_CURSOR, "CLS_C", 4, 1},
        {PA_BLINDS, "BLIND", 7, 1},
        {PA_DISSOLVE, "DSLVE", 7, 1},
        {PA_SCROLL_UP, "SC_U", 5, 1},
        {PA_SCROLL_DOWN, "SC_D", 5, 1},
        {PA_SCROLL_LEFT, "SC_L", 5, 1},
        {PA_SCROLL_RIGHT, "SC_R", 5, 1},
        {PA_SCROLL_UP_LEFT, "SC_UL", 7, 1},
        {PA_SCROLL_UP_RIGHT, "SC_UR", 7, 1},
        {PA_SCROLL_DOWN_LEFT, "SC_DL", 7, 1},
        {PA_SCROLL_DOWN_RIGHT, "SC_DR", 7, 1},
        {PA_SCAN_HORIZ, "SCANH", 4, 1},
        {PA_SCAN_VERT, "SCANV", 3, 1},
        {PA_GROW_UP, "GRW_U", 7, 1},
        {PA_GROW_DOWN, "GRW_D", 7, 1},
};

void max7219_setup()
{
  P.begin();
  P.setInvert(false);
  P.setFont(fontBG);
  P.setIntensity(5); // яркость дисплея
  max_setup = true;
}

void max_display(String msg, bool scroll, int num)
{
  max_screen[num] = msg;
  max_screen_scroll[num] = scroll;
}

void max_handle()
{
  if (!max_setup)
  {
    max7219_setup();
  }
  if (!P.displayAnimate())
  {
    max_animate = true;
    return;
  }
  if (max_screen[max_current_screen] != "")
  {
    max_speed = param(F("speed")).toInt();
    rnd = random(0, ARRAY_SIZE(catalog));
    utf8rus(max_screen[max_current_screen]).toCharArray(max_buf, 512);
    if (max_screen_scroll[max_current_screen])
    {
      P.displayScroll(max_buf, PA_LEFT, PA_SCROLL_LEFT, 70 - max_speed);
    }
    else
    {
      P.displayText(max_buf, PA_CENTER, catalog[rnd].speed * 5, 3000, catalog[rnd].effect, catalog[rnd].effect);
    }
  }
  max_animate = false;
  if (max_current_screen < 19)
    max_current_screen++;
  else
    max_current_screen = 0;
}