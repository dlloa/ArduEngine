#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H
#include <Arduboy2.h>
#include "interfaces.h"
#include "utilities/statemachine.h"
#include "controls/controller.h"
#include "utilities/enumptrmap.h"
// #include "utilities.h"

class Controller;
class ControllerList;

extern unsigned int __heap_start;
extern void *__brkval;

class SystemInfo{
public:
    int16_t ram;
    int16_t minRam;
    unsigned long startTime;
    void initMemUsage();
    void updateMemUsage();
    int16_t freeMemory();
    void StartFrame();
    unsigned long FrameTime();
};

class FrameCounter{
public:
    byte _frame = 0;
    FrameCounter();
    void Increment();
    byte GetFrame();
};

class Controllable {
public:
    ControllerList* controllerList;
    Controllable(ControllerList* inControllerList);

    void addControl(uint16_t inID, byte inControl, void (*inFunc)(void*), void* inArgs);
    void addPriorityControl(uint16_t inID, byte inControl, void (*inFunc)(void*), void* inArgs);
    virtual void takeControl() = 0;
    virtual void loseControl() = 0;
    void clearControls();
};

class RenderList{
  public:

    static const int MAX_RENDERABLES = 10; // Define a maximum number of renderables
    Renderable* aRenderables[MAX_RENDERABLES];
    int nNumRenderable = 0;
    
    void addRenderable(Renderable* inRenderable);
    void clearRenderables();
    void renderAll();

};

// // Controllable StateMachine, has controls that can be run, can take control and what not.
// template <typename T>
// class ControllableStateMachine : public StateMachine<T>, public Controllable {
// public:
//     ControllableStateMachine(ControllerList* inCL, T initialState) : 
//         StateMachine<T>(initialState), Controllable(inCL){};

//     void takeControl() override{
//         // addControl(BUTTON_JUST_PRESSED, A_BUTTON, PREV_STATE, this);
//         addControl(BUTTON_JUST_PRESSED, UP_BUTTON | DOWN_BUTTON, NEXT_STATE, this);
//     };

//     static void NEXT_STATE(void* data){
//         ControllableStateMachine* sm = static_cast<ControllableStateMachine*>(data);
//         sm -> nextState();
//     };
//     static void PREV_STATE(void* data){
//         ControllableStateMachine* sm = static_cast<ControllableStateMachine*>(data);
//         sm -> previousState();
//     };
// };

// //ControllableManager, will eventually control what Controllable has control
// template <typename T>
// class ControllableManager : public Updateable {
// public:
//     ControllableStateMachine<T> stateMachine;
//     ControllableManager(ControllerList* inCL, T initialState) : stateMachine(inCL, initialState){};
// };

// enum class ControllableManagerStates {
//     STATE_MIN,
//     GAME_START = STATE_MIN,
//     SETTINGS,
//     GAME_PLAY,
//     GAME_OVER,
//     STATE_MAX = GAME_OVER
// };


// template <>
// class ControllableManager<ControllableManagerStates> : public Updateable {
// public:
//     ControllableStateMachine<ControllableManagerStates> stateMachine;
//     EnumPtrMap<ControllableManagerStates, Controllable> controllables;

//     ControllableManager(ControllerList* inCL, FrameCounter* fc, ControllableManagerStates initialState)
//         : Updateable(fc), stateMachine(inCL, initialState) {}

//     void update() override {

//         //Checking this every frame may be expensive.
//         //not transitioned = just transitioned
//         if( !stateMachine.isTransitionFinished() ){

//             //Always clear controls, assures no bind no controls
//             stateMachine.clearControls();
//             if( controllables.has(stateMachine.getState())){
//                 //swap new controls in
//                 controllables.get(stateMachine.getState())->takeControl();
//             }

//             //TODO: Temp adding stateMachine to keep control of StateMachine!
//             stateMachine.takeControl();
            
//             //Assumes this StateMachine isn't waited upon any other objects
//             stateMachine.markTransitionFinished();
//         }


//     }
// };

class RenderableSystemInfo : public Renderable, public Updateable {
public:
    SystemInfo sysInfo;
    bool bDebugOn = true; //set false at some point
    RenderableSystemInfo(Arduboy2* inArduboy, FrameCounter* inFrameCounter);
    void render() override;
    void update() override;
};

class Animator : public Renderable, public Updateable {
  public:
    const unsigned char** sprite;
    int frames;
    int size;

    int currentframe = 0;
    int framecounter;
    int framerate;

    int posX = 0;
    int posY = 0;

    bool bAnimating = false;

    void setPosition(int inX, int inY);

    Animator(Arduboy2* inArduboy, FrameCounter* inFrameCounter, const unsigned char** inSprite, int inSize, int inFrames, int inFrameRate);

    void setSprite(const unsigned char** newSprite, int newFrames);

    void startAnimation();

    void stopAnimation();

    void update() override;

    void render() override;
};

#endif