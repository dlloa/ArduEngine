#ifndef MENUS
#define MENUS
#include <Arduboy2.h>

//Game Title Menu
const char menu0[] PROGMEM = "Start";
const char menu1[] PROGMEM = "Load";
const char menu2[] PROGMEM = "Settings";
const char* const menuItems[] PROGMEM = { menu0, menu1, menu2 };
const uint8_t menuLength = sizeof(menuItems) / sizeof(menuItems[0]);

//Counter Menu
const char countermenu0[] PROGMEM = "NOTHING";
const char countermenu1[] PROGMEM = "SAVE";

const char* const countermenuItems[] PROGMEM = { countermenu0, countermenu1 };
const uint8_t countermenuLength = sizeof(countermenuItems) / sizeof(countermenuItems[0]);


#endif