#ifndef DEBUG_MENU_H
#define DEBUG_MENU_H
#include "../gameengine.h"
#include "../pipeline.h"
#include "../updatepipeline.h"
#include "../renderpipeline.h"
#include "debugoverlay.h"
#include "../gamestates.h"
#include "../utilities/statemachine.h"
#include "../gamedata/sprites.h"

class DebugMenu : public ModalControllable, public Renderable {
public:
    bool isOpen = false;
    uint8_t currentPage = 0;
    static const uint8_t PAGE_COUNT    = 6;
    static const uint8_t PAGE_CONTROLS = 3;
    static const uint8_t PAGE_STATE    = 4;
    static const uint8_t PAGE_SPRITES  = 5;
    static const uint8_t CTRL_VISIBLE  = 5;  // entry rows visible at once

    uint8_t ctrlScrollOffset = 0;

    UpdatePipeline* updatePipeline;
    RenderPipeline* renderPipeline;
    DebugOverlay*   overlay;
    StateMachine<ControllableManagerStates>* stateMachine = nullptr;

    DebugMenu(Arduboy2* ab, ControllerList* inCL,
              UpdatePipeline* up, RenderPipeline* rp, DebugOverlay* ov)
        : ModalControllable(inCL), Renderable(ab),
          updatePipeline(up), renderPipeline(rp), overlay(ov) {}

    void takeControl() override {
        addModalControl(BUTTON_JUST_PRESSED, B_BUTTON,     CLOSE,     this);
        addModalControl(BUTTON_JUST_PRESSED, RIGHT_BUTTON, PAGE_NEXT, this);
        addModalControl(BUTTON_JUST_PRESSED, LEFT_BUTTON,  PAGE_PREV, this);
        addModalControl(BUTTON_JUST_PRESSED, A_BUTTON,     ACTION,    this);
        addModalControl(BUTTON_JUST_PRESSED, UP_BUTTON,    NAV_NEXT,  this);
        addModalControl(BUTTON_JUST_PRESSED, DOWN_BUTTON,  NAV_PREV,  this);
    }

    void loseControl() override {
        ModalControllable::loseControl(); // clearModalControls()
        isOpen = false;
    }

    void open() { if (isOpen) return; isOpen = true; currentPage = 0; ctrlScrollOffset = 0; takeControl(); }
    void close() { loseControl(); }

    void render() override {
        if (!isOpen) return;
        arduboy->fillRect(0, 0, 128, 64, BLACK);
        arduboy->drawRect(0, 0, 128, 64, WHITE);
        arduboy->setCursor(2, 0);
        switch (currentPage) {
            case 0: arduboy->print(F("< SYSTEM   >")); break;
            case 1: arduboy->print(F("< UPDATE   >")); break;
            case 2: arduboy->print(F("< RENDER   >")); break;
            case 3: arduboy->print(F("< CONTROLS >")); break;
            case 4: arduboy->print(F("< STATE    >")); break;
            case 5: arduboy->print(F("< SPRITES  >")); break;
        }
        arduboy->setCursor(0, 56);
        if (currentPage == PAGE_CONTROLS) arduboy->print(F("B:close U/D:scroll"));
        else                              arduboy->print(F("B:close  A:action"));
        switch (currentPage) {
            case 0: renderPageSystem();   break;
            case 1: renderPageUpdate();   break;
            case 2: renderPageRender();   break;
            case 3: renderPageControls(); break;
            case 4: renderPageStateNav();  break;
            case 5: renderPageSprites();   break;
        }
    }

    void renderPageSystem() {
        arduboy->setCursor(0, 8);
        arduboy->print(F("RAM:"));    arduboy->print(overlay->sysInfo.ram);
        arduboy->setCursor(0, 16);
        arduboy->print(F("MinRAM:")); arduboy->print(overlay->sysInfo.minRam);
        arduboy->setCursor(0, 24);
        arduboy->print(F("FT(us):")); arduboy->print(overlay->sysInfo.FrameTime());
        arduboy->setCursor(0, 32);
        arduboy->print(F("Overlay:")); arduboy->print(overlay->enabled ? F("ON") : F("OFF"));
        arduboy->setCursor(0, 40);
        arduboy->print(F("A=toggle overlay"));
    }

    void printEntryName(PGM_P name) {
        if (name) arduboy->print(reinterpret_cast<const __FlashStringHelper*>(name));
        else      arduboy->print('?');
    }

    void printButtons(byte mask) {
        if (mask & B_BUTTON)     arduboy->print('B');
        if (mask & A_BUTTON)     arduboy->print('A');
        if (mask & DOWN_BUTTON)  arduboy->print('D');
        if (mask & LEFT_BUTTON)  arduboy->print('L');
        if (mask & RIGHT_BUTTON) arduboy->print('R');
        if (mask & UP_BUTTON)    arduboy->print('U');
    }

    void printEventType(uint16_t flags) {
        if (flags & BUTTON_CHORD_HELD)                  { arduboy->print(F("CH"));  return; }
        if (flags & ANY_BUTTON_FRAME_DELAYED_REPEATING) { arduboy->print(F("AFR")); return; }
        if (flags & ANY_BUTTON_DELAYED_REPEATING)       { arduboy->print(F("ADR")); return; }
        if (flags & ANY_BUTTON_REPEATING)               { arduboy->print(F("AR"));  return; }
        if (flags & ANY_BUTTON_JUST_PRESSED)            { arduboy->print(F("AJP")); return; }
        if (flags & BUTTON_FRAME_DELAYED_REPEATING)     { arduboy->print(F("FR"));  return; }
        if (flags & BUTTON_DELAYED_REPEATING)           { arduboy->print(F("DR"));  return; }
        if (flags & BUTTON_REPEATING)                   { arduboy->print(F("R"));   return; }
        if (flags & BUTTON_HELD)                        { arduboy->print(F("H"));   return; }
        if (flags & BUTTON_JUST_RELEASED)               { arduboy->print(F("JR"));  return; }
        if (flags & BUTTON_JUST_PRESSED)                { arduboy->print(F("JP"));  return; }
    }

    void renderPageUpdate() {
        arduboy->setCursor(0, 8);
        arduboy->print(F("Update: ")); arduboy->print(updatePipeline->count);
        uint8_t y = 16;
        for (uint8_t i = 0; i < updatePipeline->count && y < 56; i++, y += 8) {
            arduboy->setCursor(0, y);
            arduboy->print(i); arduboy->print(':');
            printEntryName(updatePipeline->entries[i].ptr->dbgName);
            arduboy->print(F(" P")); arduboy->print(updatePipeline->entries[i].priority);
        }
    }

    void renderPageRender() {
        arduboy->setCursor(0, 8);
        arduboy->print(F("Render: ")); arduboy->print(renderPipeline->count);
        uint8_t y = 16;
        for (uint8_t i = 0; i < renderPipeline->count && y < 56; i++, y += 8) {
            arduboy->setCursor(0, y);
            arduboy->print(i); arduboy->print(':');
            printEntryName(renderPipeline->entries[i].ptr->dbgName);
            arduboy->print(F(" P")); arduboy->print(renderPipeline->entries[i].priority);
        }
    }

    void renderPageControls() {
        uint8_t total = controllerList->numControls;
        uint8_t last  = ctrlScrollOffset + CTRL_VISIBLE;
        if (last > total) last = total;
        arduboy->setCursor(0, 8);
        arduboy->print(F("Ctrl:"));
        arduboy->print(ctrlScrollOffset + 1);
        arduboy->print('-');
        arduboy->print(last);
        arduboy->print('/');
        arduboy->print(total);
        uint8_t y = 16;
        for (uint8_t i = ctrlScrollOffset; i < total && y < 57; i++, y += 8) {
            arduboy->setCursor(0, y);
            arduboy->print(i);
            arduboy->print(controllerList->listPriorities[i] >= CONTROL_PERSISTENT ? F(":P ") : F(":G "));
            printEventType(controllerList->listIDs[i]);
            arduboy->print(' ');
            printButtons(controllerList->listControls[i]);
        }
    }

    void renderPageStateNav() {
        arduboy->setCursor(0, 8);
        arduboy->print(F("State: "));
        if (stateMachine) {
            switch (stateMachine->getState()) {
                case ControllableManagerStates::GAME_START: arduboy->print(F("START")); break;
                case ControllableManagerStates::GAME_PLAY:  arduboy->print(F("PLAY"));  break;
                case ControllableManagerStates::SETTINGS:   arduboy->print(F("SETT.")); break;
                case ControllableManagerStates::GAME_OVER:  arduboy->print(F("OVER"));  break;
            }
        }
        arduboy->setCursor(0, 24);
        arduboy->print(F("UP:next  DOWN:prev"));
    }

    void renderPageSprites() {
        for (uint8_t i = 0; i < sprite_allArray_LEN; i++) {
            const unsigned char* bmp = (const unsigned char*)pgm_read_ptr(&sprite_allArray[i]);
            arduboy->drawBitmap(i * 16, 8, bmp, 16, 16, WHITE);
            arduboy->setCursor(i * 16 + 5, 26);
            arduboy->print(i);
        }
    }

    static void OPEN(void* d)      { static_cast<DebugMenu*>(d)->open(); }
    static void CLOSE(void* d)     { static_cast<DebugMenu*>(d)->close(); }
    static void PAGE_NEXT(void* d) { DebugMenu* m = static_cast<DebugMenu*>(d); if (m->currentPage < PAGE_COUNT - 1) { m->currentPage++; m->ctrlScrollOffset = 0; } }
    static void PAGE_PREV(void* d) { DebugMenu* m = static_cast<DebugMenu*>(d); if (m->currentPage > 0) { m->currentPage--; m->ctrlScrollOffset = 0; } }
    static void ACTION(void* d)    { DebugMenu* m = static_cast<DebugMenu*>(d); if (m->currentPage == 0) m->overlay->enabled = !m->overlay->enabled; }
    static void NAV_NEXT(void* d) {
        DebugMenu* m = static_cast<DebugMenu*>(d);
        if (m->currentPage == PAGE_STATE && m->stateMachine)
            m->stateMachine->nextState();
        else if (m->currentPage == PAGE_CONTROLS && m->ctrlScrollOffset > 0)
            m->ctrlScrollOffset--;
    }
    static void NAV_PREV(void* d) {
        DebugMenu* m = static_cast<DebugMenu*>(d);
        if (m->currentPage == PAGE_STATE && m->stateMachine)
            m->stateMachine->previousStateInOrder();
        else if (m->currentPage == PAGE_CONTROLS &&
                 m->ctrlScrollOffset + CTRL_VISIBLE < m->controllerList->numControls)
            m->ctrlScrollOffset++;
    }
};
#endif
