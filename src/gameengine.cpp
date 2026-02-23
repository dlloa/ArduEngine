#include "gameengine.h"
#include "managers/controller.h"

Controllable::Controllable(ControllerList* inControllerList) : controllerList(inControllerList) {}

void Controllable::addControl(uint16_t inID, byte inControl, void (*inFunc)(void*), void* inArgs) {
    controllerList->addControl(inID, inControl, inFunc, inArgs);
}

void Controllable::addPriorityControl(uint16_t inID, byte inControl, void (*inFunc)(void*), void* inArgs) {
    controllerList->addPriorityControl(inID, inControl, inFunc, inArgs);
}

void Controllable::clearControls() {
    controllerList->clearControls();
}

void ModalControllable::addModalControl(uint16_t inID, byte inControl,
                                        void (*inFunc)(void*), void* inArgs) {
    controllerList->addModalControl(inID, inControl, inFunc, inArgs);
}

void ModalControllable::loseControl() {
    controllerList->clearModalControls();
}
