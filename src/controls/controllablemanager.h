#ifndef CONTROLLABLE_MANAGER
#define CONTROLLABLE_MANAGER

#include "../gameengine.h"
#include "../utilities/enumptrmap.h"
#include "../utilities/enumptrlistmap.h"

// Controllable StateMachine, has controls that can be run, can take control and what not.
template <typename T>
class ControllableStateMachine : public StateMachine<T>, public Controllable {
public:
    ControllableStateMachine(ControllerList* inCL, T initialState) : 
        StateMachine<T>(initialState), Controllable(inCL){};

    void takeControl() override{
        // addControl(BUTTON_JUST_PRESSED, A_BUTTON, PREV_STATE, this);
        addControl(BUTTON_JUST_PRESSED, UP_BUTTON | DOWN_BUTTON, NEXT_STATE, this);
    };

    static void NEXT_STATE(void* data){
        ControllableStateMachine* sm = static_cast<ControllableStateMachine*>(data);
        sm -> nextState();
    };
    static void PREV_STATE(void* data){
        ControllableStateMachine* sm = static_cast<ControllableStateMachine*>(data);
        sm -> previousState();
    };
};

template <typename T>
class StateMachineControllableWrapper : public Controllable {
public: 
    StateMachine<T>* sm;
    StateMachineControllableWrapper(ControllerList* inCL, StateMachine<T>* inStateMachine) : Controllable(inCL), sm(inStateMachine) {};

    void takeControl() override{
        //Controls for StateMachine
        addPriorityControl(BUTTON_JUST_PRESSED, UP_BUTTON | DOWN_BUTTON, NEXT_STATE, this);
        addPriorityControl(BUTTON_JUST_PRESSED, LEFT_BUTTON | RIGHT_BUTTON, PREV_STATE, this);
    }
    void loseControl() override{

    }
    static void NEXT_STATE(void* data){
        StateMachineControllableWrapper* smw = static_cast<StateMachineControllableWrapper*>(data);
        smw -> sm -> nextState();
    };
    static void PREV_STATE(void* data){
        StateMachineControllableWrapper<T>* smw = static_cast<StateMachineControllableWrapper*>(data);
        smw -> sm -> previousStateInOrder();
    };
};

template <typename T>
class RenderableManager : public Updateable {
public:
    RenderList* renderList;
    StateMachine<T>* stateMachine;
    EnumPtrListMap<T, Renderable, 3> renderables;
    RenderableManager(FrameCounter* fc, RenderList* rl, StateMachine<T>* stateMachine) : Updateable(fc), renderList(rl), stateMachine(stateMachine){}

    void update() override {

        //Just transitioned, do everything we need to do.
        if( !(stateMachine -> isTransitionFinished()) ){

            renderList->clearRenderables();

            //iter through all possible renderable for state 
            for( uint8_t i = 0; i < renderables.size(stateMachine -> getState()); i++){
                if( renderables.has(stateMachine -> getState(), i)){
                    renderList->addRenderable(renderables.get(stateMachine -> getState(), i));
                }
            }

        }
    }
};

//ControllableManager, will eventually control what Controllable has control
template <typename T>
class ControllableManager : public Updateable {
public:

    ControllerList* cl;
    StateMachine<T>* stateMachine;
    // ControllableStateMachine<T> stateMachine;

    // EnumPtrMap<T, Controllable> controllables;
    // ControllableManager(ControllerList* inCL, FrameCounter* fc, T initialState)
    // : Updateable(fc), stateMachine(inCL, initialState) {}
    
    EnumPtrMap<T, Controllable> controllables;
    ControllableManager(ControllerList* inCL, FrameCounter* fc, StateMachine<T>* inStateMachine)
    : Updateable(fc), cl(inCL), stateMachine(inStateMachine) {}
    

    void update() override {
        //Checking this every frame may be expensive.
        //not transitioned = just transitioned
        // if( !stateMachine.isTransitionFinished() ){
        if( !stateMachine -> isTransitionFinished() ){

            //Always clear controls, assures no bind no controls
            // stateMachine.clearControls(); //statemachine can not clear controls anymore
            cl->clearControls();

            // if( controllables.has(stateMachine.getState())){
            if( controllables.has(stateMachine -> getState())){
                //swap new controls in
                // controllables.get(stateMachine.getState())->takeControl();
                controllables.get(stateMachine -> getPreviousState())->loseControl();
                controllables.get(stateMachine -> getState())->takeControl();
            }

            //TODO: Temp adding stateMachine to keep control of StateMachine!
            // stateMachine.takeControl();
            
            //Assumes this StateMachine isn't waited upon any other objects
            // stateMachine.markTransitionFinished();
        }
    }
};

template <typename StateEnum>
class StateMachineUpdateable : public StateMachine<StateEnum>, public Updateable{
public:

    StateMachineUpdateable(FrameCounter* inFC, StateEnum initialState) :  StateMachine<StateEnum>(initialState), Updateable(inFC) {

    }
    void update() override{
        if( !isTransitionFinished() ){
            markTransitionFinished();
        }
    }
};

#endif