#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <Arduboy2.h>
#include "interfaces.h"
// #include "gameengine.h"

class Renderable;
class Updateable;
class FrameCounter;
class ControllerList;

//Control specific repeatDelayFrames?
  //Currently all Frame Delayed Repeating input is using the same count of frames
  //How much would it change to have each control determine the amount of frames needed to trigger controls
  //May be a lift since update currently sets the bit for what is repeating
  //Having the check use the frame counter to check the Timers on control isID

//Controller Button Memory may be better off being separate from Controller
  //Games can use it as needed. Memory usage reduction.

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
  ANY_BUTTON_FRAME_DELAYED_REPEATING  = 1 << 9
};

//Update to Updateable? Should be an easy change.
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
    byte buttonTimers[8] = {0,0,0,0,0,0,0,0};
    byte repeatDelayButtonTimers[8] = {0,0,0,0,0,0,0,0};

    //Button Memory, for button combos //TO BE REMOVED
    static const byte buttonMemorySize = 10;
    byte buttonMemory[buttonMemorySize];
    byte buttonMemoryFrames = 30;
    byte buttonMemoryCurrentFrames = 0;
    bool memoryCleared = true;

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
    String debugPrint();
    String quickPrint();

};


class ControllerMemory : public Renderable, public Updateable {
public:

    static const byte buttonMemorySize = 10;
    byte buttonMemory[buttonMemorySize];
    byte buttonMemoryFrames = 30;
    byte buttonMemoryCurrentFrames = 0;
    bool memoryCleared = true;
    Controller* controller;

    ControllerMemory(Arduboy2* inArduboy, FrameCounter* inFrameCounter, Controller* inController);

    void render() override;
    void update() override;

    void addButtonMemory(byte inMemory);
    void clearButtonMemory();
    void moveButtonMemory();
    String memoryPrint();
};

class ControllerList{
  public:
    typedef void (*ControlFunction)(void*);
    Controller* controller;

    static const int MAX_CONTROLS = 12;
    uint16_t listIDs[MAX_CONTROLS];
    uint16_t listControls[MAX_CONTROLS];
    ControlFunction funcControls[MAX_CONTROLS];
    void* funcArgs[MAX_CONTROLS];
    byte numControls = 0;
    byte priorityControls = 0;

    ControllerList(Controller* inController);
    void addControl(uint16_t inID, byte inButtons, ControlFunction inFunc, void* inArgs);
    void addPriorityControl(uint16_t inID, byte inButtons, ControlFunction inFunc, void* inArgs);
    void clearControls();
    void runControls();
    byte getNumControls();
    String debugOutput();

};

#endif