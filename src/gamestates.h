#ifndef GAME_STATES_H
#define GAME_STATES_H

// STATE_MIN and STATE_MAX are required sentinels for EnumPtrMap / EnumPtrListMap.
enum class ControllableManagerStates {
    STATE_MIN,
    GAME_START = STATE_MIN, // Title screen / state selector
    SETTINGS,               // Settings / options view
    GAME_PLAY,              // Active gameplay (counter)
    GAME_OVER,              // Debug / system info overlay
    STATE_MAX = GAME_OVER
};

#endif
