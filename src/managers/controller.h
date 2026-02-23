#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <Arduboy2.h>
#include "base.h"

class Renderable;
class Updateable;
class FrameCounter;
class ControllerList;

enum ControllerID : uint16_t {
  NONE                                = 0,
  BUTTON_JUST_PRESSED                 = 1 << 0,   // 0b00000001
  BUTTON_JUST_RELEASED                = 1 << 1,   // 0b00000010
  BUTTON_HELD                         = 1 << 2,   // 0b00000100
  BUTTON_REPEATING                    = 1 << 3,   // 0b00001000
  BUTTON_DELAYED_REPEATING            = 1 << 4,   // 0b00010000
  BUTTON_FRAME_DELAYED_REPEATING      = 1 << 5,   // 0b00100000
  ANY_BUTTON_JUST_PRESSED             = 1 << 6,   // 0b01000000
  ANY_BUTTON_REPEATING                = 1 << 7,   // 0b10000000
  ANY_BUTTON_DELAYED_REPEATING        = 1 << 8,
  ANY_BUTTON_FRAME_DELAYED_REPEATING  = 1 << 9,
  BUTTON_CHORD_HELD                   = 1 << 10  // all mask buttons held for chordHoldThreshold frames (fires once)
};

class Controller{
  public:
    //Button Values
    byte prevButtons = 0;
    byte currButtons = 0;
    byte currHeld = 0;
    byte currPressed = 0;
    byte currReleased = 0;
    byte currRepeating = 0;
    byte currDelayedRepeating = 0;

    //Timers for Repeating Inputs
    byte frameCounter = 0;
    byte repeatDelayFrames = 10;
    // Tunable: frames all chord buttons must be simultaneously held before BUTTON_CHORD_HELD fires.
    byte chordHoldThreshold = 60;
    // One slot per button bit; counts held frames until repeatDelayFrames is reached.
    byte buttonTimers[8] = {0,0,0,0,0,0,0,0};
    // Counts frames after the hold threshold; wraps to produce a periodic pulse.
    byte repeatDelayButtonTimers[8] = {0,0,0,0,0,0,0,0};

    //Button Memory: records each just-pressed event; cleared after buttonMemoryFrames idle frames.
    static const byte buttonMemorySize = 8;
    byte buttonMemory[buttonMemorySize];        // [0]=oldest, [count-1]=newest
    byte buttonMemoryFrames = 30;               // idle frames before auto-clear (1 s at 30 fps)
    byte buttonMemoryCurrentFrames = 0;         // frames elapsed since last press
    byte buttonMemoryCount = 0;                 // valid entries in buttonMemory

    Controller();
    Controller(byte inRepeatDelayFrames);
    void setRepeatDelay(byte inFrames);
    byte getRepeatDelay();

    void update(byte inCurrentButtons);

    bool isID(ControllerID inID, byte buttonToCheck, byte inFrames = 1);
    bool isIDMasked(uint16_t inFlags, byte buttonToCheck, byte inFrames);

    bool isButtonJustPressed(byte buttonToCheck);
    bool isButtonJustReleased(byte buttonToCheck);
    bool isButtonHeld(byte buttonToCheck);
    bool isButtonRepeating(byte buttonToCheck);
    bool isButtonDelayedRepeating(byte buttonToCheck, byte framesToDelay);
    bool isButtonFrameDelayedRepeating(byte buttonToCheck, byte framesToDelay);
    bool isAnyButtonJustPressed(byte buttonToCheck);
    bool isAnyButtonRepeating(byte buttonToCheck);
    bool isAnyButtonDelayedRepeating(byte buttonToCheck, byte framesToDelay);
    bool isAnyButtonFrameDelayedRepeating(byte buttonToCheck, byte framesToDelay);
    bool isButtonChordHeld(byte buttonMask);

};

class ControllerList{
  public:
    typedef void (*ControlFunction)(void*);
    Controller* controller;

    // Tunable: maximum simultaneous control bindings in ControllerList.
    // Current minimum at 10: 4 persistent (sm_main ×2, counter, debugMenu) +
    // 6 game (counter GAME_PLAY state). Increase if adding more controls.
    static const uint8_t MAX_CONTROLS = 10;
    // 0 = CONTROL_GAME (cleared on state transition), 1 = CONTROL_PERSISTENT (survives).
    uint8_t listPriorities[MAX_CONTROLS];
    uint16_t listIDs[MAX_CONTROLS];
    byte listControls[MAX_CONTROLS];
    ControlFunction funcControls[MAX_CONTROLS];
    void* funcArgs[MAX_CONTROLS];
    byte numControls = 0;

    // Modal control list: when non-empty, runControls() runs this list instead of the normal list.
    // Tunable: minimum at 6 — DebugMenu uses 6 (B close, L/R page, A action, UP/DOWN state nav).
    static const uint8_t MAX_MODAL_CONTROLS = 6;
    uint16_t         modalListIDs[MAX_MODAL_CONTROLS];
    byte             modalListControls[MAX_MODAL_CONTROLS];
    ControlFunction  modalFuncControls[MAX_MODAL_CONTROLS];
    void*            modalFuncArgs[MAX_MODAL_CONTROLS];
    uint8_t          numModalControls = 0;

    ControllerList(Controller* inController);
    void addControl(uint16_t inID, byte inButtons, ControlFunction inFunc, void* inArgs);
    void addPriorityControl(uint16_t inID, byte inButtons, ControlFunction inFunc, void* inArgs);
    void clearControls();
    void runControls();
    byte getNumControls();
    void addModalControl(uint16_t inID, byte inButtons, ControlFunction inFunc, void* inArgs);
    void clearModalControls();

};

#endif
