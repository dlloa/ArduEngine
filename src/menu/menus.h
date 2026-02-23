#ifndef MENUS
#define MENUS
#include <Arduboy2.h>

//Game Title Menu
const char menu0[] PROGMEM = "Start";
const char menu1[] PROGMEM = "Load";
const char menu2[] PROGMEM = "Settings";
const char* const menuItems[] PROGMEM = { menu0, menu1, menu2 };
const uint8_t menuLength = sizeof(menuItems) / sizeof(menuItems[0]);

//State Setter
const char statemenu0[] PROGMEM = "Title";
const char statemenu1[] PROGMEM = "Settings";
const char statemenu2[] PROGMEM = "Gameplay";
const char statemenu3[] PROGMEM = "Game Over!";

const char* const statemenuItems[] PROGMEM = { statemenu0, statemenu1, statemenu2, statemenu3 };
const uint8_t statemenuLength = sizeof(statemenuItems) / sizeof(statemenuItems[0]);

//Counter Menu
const char countermenu0[] PROGMEM = "NOTHING";
const char countermenu1[] PROGMEM = "SAVE";

const char* const countermenuItems[] PROGMEM = { countermenu0, countermenu1 };
const uint8_t countermenuLength = sizeof(countermenuItems) / sizeof(countermenuItems[0]);

/**
 * 
enum class ControllableManagerStates {
    STATE_MIN,
    GAME_START = STATE_MIN,
    SETTINGS,
    GAME_PLAY,
    GAME_OVER,
    STATE_MAX = GAME_OVER
};
 */

#endif