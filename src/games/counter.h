#ifndef COUNTER
#define COUNTER

#include "../gameengine.h"
#include "../controls/controller.h"
#include "../utilities/utilities.h"

enum class CounterStates {
    STATE_MIN,
    ACTIVE = STATE_MIN,
    INACTIVE,
    STATE_MAX = INACTIVE
};

class Counter : public Controllable, public Renderable, public Updateable {
public:
    
    uint8_t inner_count = 0;
    uint32_t count = 0;
    uint8_t count_timer = 100;
    Menu counter_menu = Menu(arduboy, controllerList, 2, countermenuItems);
    StateMachine<CounterStates> menu_sm = StateMachine<CounterStates>(CounterStates::INACTIVE);

    Counter(Arduboy2* inArduboy, ControllerList* inControllerList, FrameCounter* inFrameCounter):
        Controllable(inControllerList), Renderable(inArduboy), Updateable(inFrameCounter){
            counter_menu.setPosition(64, 0);
        };
    void render() override{
        //display in Top Left
        arduboy->setCursor(0,0);

        //4,294,967,295
        String num = String(count);
        int num_digits = num.length();

        String s;
        if( num_digits <= 6 ){
            s.reserve(6);
            for (uint8_t i = num.length(); i < 6; i++) {
                s += '0';
            }
        }
        else{
            s.reserve(10);
            for (uint8_t i = num.length(); i < 10; i++) {
                s += '0';
            }
        }
        s += num;

        //6 numbers (5x7 font) + 1 spacer between chars
        //6x6 36 max - 1 buffer for last character?
        int bar_length = (35 * inner_count) / 100;
        arduboy->fillRect(0, 8, bar_length, 7, WHITE);

        arduboy->print(s);

        if( menu_sm.getState() == CounterStates::ACTIVE ){
            counter_menu.render();
        }

    }
    void update() override{
        //anything outside of controls that need to update
        inner_count++;
        if( inner_count >= count_timer ){
            count++;
            inner_count -= count_timer;
        }


    }
    void takeControl() override{
        //Move Menu move selection
        addControl(BUTTON_JUST_PRESSED, 
            UP_BUTTON /* | B_BUTTON | LEFT_BUTTON | UP_BUTTON | RIGHT_BUTTON | DOWN_BUTTON */, 
            MENU_UP, this);
        addControl(BUTTON_JUST_PRESSED, 
            DOWN_BUTTON /* | B_BUTTON | LEFT_BUTTON | UP_BUTTON | RIGHT_BUTTON | DOWN_BUTTON */, 
            MENU_DOWN, this);

        //Menu selection
        addControl(BUTTON_JUST_PRESSED, 
            B_BUTTON, MAKE_SELECTION, this);
        addControl(BUTTON_JUST_PRESSED, 
            A_BUTTON, UNSELECT, this);

        //menu activation
        addControl(BUTTON_DELAYED_REPEATING, 
            DOWN_BUTTON, ACTIVATE_MENU, this);
        addControl(BUTTON_DELAYED_REPEATING, 
            UP_BUTTON, DEACTIVATE_MENU, this);
    }

    void loseControl() override{
        menu_sm.setState(CounterStates::INACTIVE);
    }

    void takePriorityControl(){
        //Any button increases the counter.
        addPriorityControl(ANY_BUTTON_JUST_PRESSED, 
            A_BUTTON | B_BUTTON | LEFT_BUTTON | UP_BUTTON | RIGHT_BUTTON | DOWN_BUTTON, 
            INCREMENT, this);
    }

    static void INCREMENT(void* data){
        Counter* c = static_cast<Counter*>(data);
        c -> count++;
    };

    static void MENU_UP(void* data){
        Counter* c = static_cast<Counter*>(data);
        c->counter_menu.MoveUp();
    }
    static void MENU_DOWN(void* data){
        Counter* c = static_cast<Counter*>(data);
        c->counter_menu.MoveDown();
    }
    static void MAKE_SELECTION(void* data){
        Counter* c = static_cast<Counter*>(data);
        c->counter_menu.Select();
    }
    static void UNSELECT(void* data){
        Counter* c = static_cast<Counter*>(data);
        if( c->counter_menu.getMadeSelection() ){ 
            c->counter_menu.Unselect();
        }
        if( c->menu_sm.getState() == CounterStates::ACTIVE){
        }
        c->menu_sm.setState(CounterStates::INACTIVE);
    }
    static void ACTIVATE_MENU(void* data){
        Counter* c = static_cast<Counter*>(data);
        c->menu_sm.setState(CounterStates::ACTIVE);
    }
    static void DEACTIVATE_MENU(void* data){
        Counter* c = static_cast<Counter*>(data);
        c->menu_sm.setState(CounterStates::INACTIVE);
    }
};

#endif