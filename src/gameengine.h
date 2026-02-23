#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "base.h"
#include "framecounter.h"
#include "systeminfo.h"
#include "pipeline.h"
#include "updatepipeline.h"
#include "renderpipeline.h"
#include "animator.h"
#include "utilities/statemachine.h"
#include "utilities/enumptrmap.h"
#include "managers/controller.h"

class Controller;
class ControllerList;

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

// A Controllable variant whose controls are registered into the ControllerList's
// modal list. When modal controls are present, runControls() runs them exclusively
// instead of the normal list. loseControl() clears the modal list.
class ModalControllable : public Controllable {
public:
    ModalControllable(ControllerList* inCL) : Controllable(inCL) {}
    void addModalControl(uint16_t inID, byte inControl, void (*inFunc)(void*), void* inArgs);
    void loseControl() override; // calls clearModalControls()
    // takeControl() still pure virtual — subclasses implement it
};

#endif
