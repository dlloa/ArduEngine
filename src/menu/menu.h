#ifndef MENU_H
#define MENU_H

#include "gameengine.h"


class Menu : public Controllable, public Renderable {
public:
    Menu(Arduboy2* arduboy, ControllerList* inControllerList, uint8_t inMaxSelection, const char* const* menuItems);
    void takeControl() override;
    void loseControl() override;
    void render() override;

    void MoveUp();
    void MoveDown();
    void Select();
    void Unselect();

    static void MOVE_UP(void* data);
    static void MOVE_DOWN(void* data);
    static void MAKE_SELECTION(void* data);
    static void UNSELECT(void* data);

    void setPosition(byte inXPos, byte inYPos);
    uint8_t getSelection();
    byte getMadeSelection();

    const char* const* items;
    uint8_t currentSelection = 0;
    uint8_t maxSelection;
    uint8_t xPos = 0;
    uint8_t yPos = 0;
    bool madeSelection = false;
};

// Maps a menu selection index to a state enum value via stateFromIndex<T>(),
// then applies that state to the bound StateMachine.
template <typename T>
class MenuStateSetter : public Menu, public Updateable {
public:
    StateMachine<T>* stateMachine;

    MenuStateSetter(Arduboy2* arduboy,
        ControllerList* inCL,
        uint8_t inMaxSelection,
        const char* const* menuItems,
        StateMachine<T>* inStateMachine,
        FrameCounter* fc)
        : Menu(arduboy, inCL, inMaxSelection, menuItems), Updateable(fc), stateMachine(inStateMachine){};

    void update() override{
        if( madeSelection ){
            uint8_t selection = getSelection();
            stateMachine->setState(stateFromIndex<T>(selection));
            madeSelection = false;
        }
    }

};

#endif