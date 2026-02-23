#ifndef CONTROLLABLE_STATE_MACHINE_H
#define CONTROLLABLE_STATE_MACHINE_H

#include "../gameengine.h"
#include "../utilities/statemachine.h"

template <typename T>
class ControllableStateMachine : public StateMachine<T>, public Controllable {
public:
    ControllableStateMachine(ControllerList* inCL, T initialState) :
        StateMachine<T>(initialState), Controllable(inCL) {};

    void takeControl() override {
        addControl(BUTTON_JUST_PRESSED, UP_BUTTON | DOWN_BUTTON, NEXT_STATE, this);
    };
    void loseControl() override {};

    static void NEXT_STATE(void* data) {
        ControllableStateMachine* sm = static_cast<ControllableStateMachine*>(data);
        sm->nextState();
    };
    static void PREV_STATE(void* data) {
        ControllableStateMachine* sm = static_cast<ControllableStateMachine*>(data);
        sm->previousState();
    };
};

template <typename T>
class StateMachineControllableWrapper : public Controllable {
public:
    StateMachine<T>* sm;
    StateMachineControllableWrapper(ControllerList* inCL, StateMachine<T>* inStateMachine) : Controllable(inCL), sm(inStateMachine) {};

    // Registers as priority (persistent) controls: UP/DOWN advance state, LEFT/RIGHT go back.
    void takeControl() override {
        addPriorityControl(BUTTON_JUST_PRESSED, UP_BUTTON | DOWN_BUTTON, NEXT_STATE, this);
        addPriorityControl(BUTTON_JUST_PRESSED, LEFT_BUTTON | RIGHT_BUTTON, PREV_STATE, this);
    }
    void loseControl() override {}

    static void NEXT_STATE(void* data) {
        StateMachineControllableWrapper* smw = static_cast<StateMachineControllableWrapper*>(data);
        smw->sm->nextState();
    };
    static void PREV_STATE(void* data) {
        StateMachineControllableWrapper<T>* smw = static_cast<StateMachineControllableWrapper*>(data);
        smw->sm->previousStateInOrder();
    };
};

#endif
