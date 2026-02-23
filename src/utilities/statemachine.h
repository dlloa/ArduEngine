#ifndef STATEMACHINE
#define STATEMACHINE

template <typename StateEnum>
class StateMachine {
public:
    StateMachine(StateEnum initialState)
        : currentState(initialState), bTransitionFinished(false) {}

    void setState(StateEnum newState) {
        if (isValidState(newState)) {
            previousState = currentState;
            currentState = newState;
            bTransitionFinished = false;
        }
    }

    StateEnum getState() const {
        return currentState;
    }

    StateEnum getPreviousState() const {
        return previousState;
    }

    bool previousIsCurrentState() const {
        return currentState == previousState;
    }

    void nextState() {
        int next = static_cast<int>(currentState) + 1;
        if (next > static_cast<int>(StateEnum::STATE_MAX)) {
            next = static_cast<int>(StateEnum::STATE_MIN);
        }
        currentState = static_cast<StateEnum>(next);
        bTransitionFinished = false;
    }

    void previousStateInOrder() {
        int prev = static_cast<int>(currentState) - 1;
        if (prev < static_cast<int>(StateEnum::STATE_MIN)) {
            prev = static_cast<int>(StateEnum::STATE_MAX);
        }
        currentState = static_cast<StateEnum>(prev);
        bTransitionFinished = false;
    }

    bool isState(StateEnum state) const {
        return currentState == state;
    }

    bool isTransitionFinished() const {
        return bTransitionFinished;
    }

    void markTransitionFinished() {
        bTransitionFinished = true;
    }

    static void nextStateWrapper(void* data) {
        static_cast<StateMachine*>(data)->nextState();
    }

    static void previousStateWrapper(void* data) {
        static_cast<StateMachine*>(data)->previousState();
    }

private:
    StateEnum currentState;
    StateEnum previousState;
    bool bTransitionFinished;

    bool isValidState(StateEnum state) const {
        return static_cast<int>(state) >= static_cast<int>(StateEnum::STATE_MIN) &&
               static_cast<int>(state) <= static_cast<int>(StateEnum::STATE_MAX);
    }
};

#endif