#ifndef CONTROLLABLE_MANAGER
#define CONTROLLABLE_MANAGER

#include "../gameengine.h"
#include "../utilities/enumptrmap.h"
#include "../utilities/enumptrlistmap.h"
#include "controllablestatemachine.h"
#include "renderablemanager.h"
#include "statemachineupdateable.h"
#include "updateablemanager.h"
#include "scenemanager.h"

template <typename T>
class ControllableManager : public Updateable {
public:
    ControllerList* cl;
    StateMachine<T>* stateMachine;
    EnumPtrMap<T, Controllable> controllables;
    ControllableManager(ControllerList* inCL, FrameCounter* fc, StateMachine<T>* inStateMachine)
        : Updateable(fc), cl(inCL), stateMachine(inStateMachine) {}

    void update() override {
        // Only runs when a state transition is pending (not every frame).
        // loseControl() is called on the old state before takeControl() on the new one.
        if (!stateMachine->isTransitionFinished()) {
            cl->clearControls();
            if (controllables.has(stateMachine->getState())) {
                if (controllables.has(stateMachine->getPreviousState()))
                    controllables.get(stateMachine->getPreviousState())->loseControl();
                controllables.get(stateMachine->getState())->takeControl();
            }
        }
    }
};

#endif
