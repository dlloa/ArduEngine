#ifndef STATE_MACHINE_UPDATEABLE_H
#define STATE_MACHINE_UPDATEABLE_H

#include "../gameengine.h"
#include "../utilities/statemachine.h"

// Wraps a StateMachine as an Updateable. Each frame, if a transition is pending,
// it calls markTransitionFinished() so the scene managers know to update next frame.
template <typename StateEnum>
class StateMachineUpdateable : public StateMachine<StateEnum>, public Updateable {
public:
    StateMachineUpdateable(FrameCounter* inFC, StateEnum initialState)
        : StateMachine<StateEnum>(initialState), Updateable(inFC) {}

    void update() override {
        if (!isTransitionFinished()) {
            markTransitionFinished();
        }
    }
};

#endif
