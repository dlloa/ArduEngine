#ifndef COUNTER
#define COUNTER

#include "../gameengine.h"
#include "../managers/controller.h"
#include "../utilities/utilities.h"
#include "../saves.h"
#include <Tinyfont.h>

enum class CounterStates {
    STATE_MIN,
    ACTIVE = STATE_MIN,
    INACTIVE,
    STATE_MAX = INACTIVE
};

class Counter : public Controllable, public Renderable, public Updateable {
public:
    // inner_count accumulates each frame; count increments once inner_count reaches count_timer.
    uint8_t inner_count = 0;
    uint32_t count = 0;
    uint8_t count_timer = 100;
    // Auto-save: write to EEPROM every AUTO_SAVE_INTERVAL frames (~30 s at 30 fps).
    static const uint16_t AUTO_SAVE_INTERVAL = 900;
    uint16_t autoSaveTimer = 0;
    Tinyfont tinyFont{Arduboy2Base::sBuffer, Arduboy2::width(), Arduboy2::height()};
    Menu counter_menu = Menu(arduboy, controllerList, 2, countermenuItems);
    // ACTIVE = menu overlay visible; INACTIVE = hidden.
    StateMachine<CounterStates> menu_sm = StateMachine<CounterStates>(CounterStates::INACTIVE);

    void saveData() {
        CounterSave s = { count };
        SaveSystem::save(SAVED_COUNTER, s);
    }
    void loadData() {
        CounterSave s;
        if (SaveSystem::load(SAVED_COUNTER, s)) {
            count = s.count;
        }
    }

    Counter(Arduboy2* inArduboy, ControllerList* inControllerList, FrameCounter* inFrameCounter):
        Controllable(inControllerList), Renderable(inArduboy), Updateable(inFrameCounter){
            counter_menu.setPosition(64, 0);
        };
    void render() override{
        arduboy->setCursor(0,0);

        // Stack-allocated buffer; no heap. <=6 digits: pad to 6, >6 digits: pad to 16.
        char buf[17];                          // 16 digits + null terminator
        ltoa(count, buf, 10);
        uint8_t num_digits = strlen(buf);
        uint8_t padLen = (num_digits <= 6) ? 6 : 16;
        for (uint8_t i = num_digits; i < padLen; i++) arduboy->print('0');
        arduboy->print(buf);

        // 35 = pixel width of the progress bar.
        int bar_length = (35 * inner_count) / count_timer;
        arduboy->fillRect(0, 8, bar_length, 7, WHITE);

        // Percentage readout in tiny font, right of the progress bar.
        uint8_t pct = (uint16_t(inner_count) * 100) / count_timer;
        tinyFont.setCursor(37, 9);
        tinyFont.print(pct);
        tinyFont.print('%');

        // Button memory: show recent input sequence below the progress bar.
        Controller* ctrl = controllerList->controller;
        if (ctrl->buttonMemoryCount > 0) {
            tinyFont.setCursor(0, 17);
            for (byte i = 0; i < ctrl->buttonMemoryCount; i++) {
                if (i > 0) tinyFont.print(' ');
                byte m = ctrl->buttonMemory[i];
                if (m & B_BUTTON)    tinyFont.print('B');
                if (m & A_BUTTON)    tinyFont.print('A');
                if (m & DOWN_BUTTON) tinyFont.print('D');
                if (m & LEFT_BUTTON) tinyFont.print('L');
                if (m & RIGHT_BUTTON)tinyFont.print('R');
                if (m & UP_BUTTON)   tinyFont.print('U');
            }
        }

        if( menu_sm.getState() == CounterStates::ACTIVE ){
            counter_menu.render();
        }

    }
    void update() override{
        inner_count++;
        if( inner_count >= count_timer ){
            count++;
            inner_count -= count_timer;
        }
        // Auto-save on interval.
        if (++autoSaveTimer >= AUTO_SAVE_INTERVAL) {
            autoSaveTimer = 0;
            saveData();
        }
    }
    void takeControl() override{
        addControl(BUTTON_JUST_PRESSED, UP_BUTTON, MENU_UP, this);
        addControl(BUTTON_JUST_PRESSED, DOWN_BUTTON, MENU_DOWN, this);

        addControl(BUTTON_JUST_PRESSED, B_BUTTON, MAKE_SELECTION, this);
        addControl(BUTTON_JUST_PRESSED, A_BUTTON, UNSELECT, this);

        addControl(BUTTON_DELAYED_REPEATING, DOWN_BUTTON, ACTIVATE_MENU, this);
        addControl(BUTTON_DELAYED_REPEATING, UP_BUTTON, DEACTIVATE_MENU, this);
    }

    void loseControl() override{
        menu_sm.setState(CounterStates::INACTIVE);
    }

    // Registered as a persistent (priority) control — survives all state transitions.
    void takePriorityControl(){
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
        if (c->counter_menu.getSelection() == 1) { // SAVE
            c->saveData();
        }
        c->menu_sm.setState(CounterStates::INACTIVE);
    }
    // Dismisses the menu overlay and deactivates menu_sm.
    static void UNSELECT(void* data){
        Counter* c = static_cast<Counter*>(data);
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
