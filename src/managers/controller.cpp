#ifndef CONTROLLER
#define CONTROLLER
#include "controller.h"
#include "gameengine.h"

Controller::Controller(){

}

Controller::Controller(byte inRepeatDelayFrames){
  repeatDelayFrames = inRepeatDelayFrames;
}

void Controller::setRepeatDelay(byte inFrames){
  repeatDelayFrames = inFrames;
}

byte Controller::getRepeatDelay(){
  	return repeatDelayFrames;
}

void Controller::update(byte inCurrentButtons){
	frameCounter++;

	//Updating different button states
	prevButtons = currButtons;
	currButtons = inCurrentButtons;
	currHeld = prevButtons & currButtons;
	currReleased = (prevButtons ^ currButtons) & prevButtons;
	currPressed = (prevButtons ^ currButtons) & currButtons;

	// Two-phase repeat timer per button bit:
	//   Phase 1 — buttonTimers fills up to repeatDelayFrames (hold threshold).
	//   Phase 2 — repeatDelayButtonTimers cycles 0..repeatDelayFrames, producing
	//             a single-frame pulse each cycle for delayed-repeat events.
	for( byte bit = 0; bit < 8; bit++ )
  {
  	if( currHeld & ( 1 << bit ) ){
    	  if( buttonTimers[bit] < 255){
    	    buttonTimers[bit]++;
    	  }
    	  if( buttonTimers[bit] >= repeatDelayFrames ){
    	    repeatDelayButtonTimers[bit]++;
    	    if( repeatDelayButtonTimers[bit] >= repeatDelayFrames ){
    	      repeatDelayButtonTimers[bit] = 0;
    	    }
    	  }
    	}

    //If released, clear timer
    if( currReleased & (1 << bit)){
      buttonTimers[bit] = 0;
      repeatDelayButtonTimers[bit] = 0;
    }

    if( buttonTimers[bit] >= repeatDelayFrames ){
      currRepeating |= 1 << bit;
    }
    else{
      byte mask = 0b00000001 << bit;
      mask = ~mask;
      currRepeating &= mask;
    }

    // Fires on the frame just before the delayed-repeat timer wraps,
    // producing exactly one pulse per cycle.
    if( repeatDelayButtonTimers[bit] == repeatDelayFrames - 1 ){
      currDelayedRepeating |= 1 << bit;
    }
    else{
      byte mask = 0b00000001 << bit;
      mask = ~mask;
      currDelayedRepeating &= mask;
    }

  }

  // Button memory: record each just-pressed event; idle-clear after timeout.
  if (currPressed) {
    if (buttonMemoryCount < buttonMemorySize) {
      buttonMemory[buttonMemoryCount] = currPressed;
      buttonMemoryCount++;
    } else {
      for (byte i = 0; i < buttonMemorySize - 1; i++)
        buttonMemory[i] = buttonMemory[i + 1];
      buttonMemory[buttonMemorySize - 1] = currPressed;
    }
    buttonMemoryCurrentFrames = 0;
  } else if (buttonMemoryCount > 0) {
    if (++buttonMemoryCurrentFrames >= buttonMemoryFrames) {
      buttonMemoryCount = 0;
      buttonMemoryCurrentFrames = 0;
    }
  }

}

bool Controller::isID(ControllerID inID, byte buttonToCheck, byte inFrames = 1){
  bool toReturn = false;
  switch(inID){
    case BUTTON_JUST_PRESSED:
      toReturn = isButtonJustPressed(buttonToCheck);
      break;
    case BUTTON_JUST_RELEASED:
      toReturn = isButtonJustReleased(buttonToCheck);
      break;
    case BUTTON_HELD:
      toReturn = isButtonHeld(buttonToCheck);
      break;
    case BUTTON_REPEATING:
      toReturn = isButtonRepeating(buttonToCheck);
      break;
    case BUTTON_DELAYED_REPEATING:
      toReturn = isButtonDelayedRepeating(buttonToCheck, inFrames);
      break;
    case ANY_BUTTON_JUST_PRESSED:
      toReturn = isAnyButtonJustPressed(buttonToCheck);
      break;
    case ANY_BUTTON_REPEATING:
      toReturn = isAnyButtonRepeating(buttonToCheck);
      break;
    case ANY_BUTTON_DELAYED_REPEATING:
      toReturn = isAnyButtonDelayedRepeating(buttonToCheck, inFrames);
      break;
  }
  return toReturn;
}

bool Controller::isIDMasked(uint16_t inFlags, byte buttonToCheck, byte inFrames){
  bool toReturn = false;
  bool inButtonJustPressed = (inFlags & BUTTON_JUST_PRESSED) > 0;
  bool inButtonJustReleased = (inFlags & BUTTON_JUST_RELEASED) > 0;
  bool inButtonHeld = (inFlags & BUTTON_HELD) > 0;
  bool inButtonRepeating = (inFlags & BUTTON_REPEATING) > 0;
  bool inButtonDelayedRepeating = (inFlags & BUTTON_DELAYED_REPEATING) > 0;
  bool inButtonFramedDelayedRepeating = (inFlags & BUTTON_FRAME_DELAYED_REPEATING) > 0;
  bool inAnyButtonJustPressed = (inFlags & ANY_BUTTON_JUST_PRESSED) > 0;
  bool inAnyButtonRepeating = (inFlags & ANY_BUTTON_REPEATING) > 0;
  bool inAnyButtonDelayedRepeating = (inFlags & ANY_BUTTON_DELAYED_REPEATING) > 0;
  bool inAnyButtonFrameDelayedRepeating = (inFlags & ANY_BUTTON_FRAME_DELAYED_REPEATING) > 0;

  toReturn = inButtonJustPressed && isButtonJustPressed(buttonToCheck) ? true : toReturn;
  toReturn = inButtonJustReleased && isButtonJustReleased(buttonToCheck) ? true : toReturn;
  toReturn = inButtonHeld && isButtonHeld(buttonToCheck) ? true : toReturn;
  toReturn = inButtonRepeating && isButtonRepeating(buttonToCheck) ? true : toReturn;
  toReturn = inButtonDelayedRepeating && isButtonDelayedRepeating(buttonToCheck, inFrames) ? true : toReturn;
  toReturn = inButtonFramedDelayedRepeating && isButtonFrameDelayedRepeating(buttonToCheck, inFrames) ? true : toReturn;
  toReturn = inAnyButtonJustPressed && isAnyButtonJustPressed(buttonToCheck) ? true : toReturn;
  toReturn = inAnyButtonRepeating && isAnyButtonRepeating(buttonToCheck) ? true : toReturn;
  toReturn = inAnyButtonDelayedRepeating && isAnyButtonDelayedRepeating(buttonToCheck, inFrames) ? true : toReturn;
  toReturn = inAnyButtonFrameDelayedRepeating && isAnyButtonFrameDelayedRepeating(buttonToCheck, inFrames) ? true : toReturn;
  bool inButtonChordHeld = (inFlags & BUTTON_CHORD_HELD) > 0;
  toReturn = inButtonChordHeld && isButtonChordHeld(buttonToCheck) ? true : toReturn;
  return toReturn;
}

bool Controller::isButtonJustPressed(byte buttonToCheck){
  return (currPressed & buttonToCheck) == buttonToCheck;
}
bool Controller::isButtonJustReleased(byte buttonToCheck){
  return (currReleased & buttonToCheck) == buttonToCheck;
}
bool Controller::isButtonHeld(byte buttonToCheck){
  return (currHeld & buttonToCheck) == buttonToCheck;
}
bool Controller::isButtonRepeating(byte buttonToCheck){
  return (currRepeating & buttonToCheck) == buttonToCheck;
}
bool Controller::isButtonDelayedRepeating(byte buttonToCheck, byte framesToDelay){
  return (frameCounter % framesToDelay == 0) && isButtonRepeating(buttonToCheck);
}

bool Controller::isButtonFrameDelayedRepeating(byte buttonToCheck, byte framesToDelay){
  return (frameCounter % framesToDelay == 0) && (currDelayedRepeating & buttonToCheck) == buttonToCheck;
}

bool Controller::isAnyButtonJustPressed(byte buttonToCheck){
  return currPressed & buttonToCheck;
}
bool Controller::isAnyButtonRepeating(byte buttonToCheck){
  return (currRepeating & buttonToCheck);
}
bool Controller::isAnyButtonDelayedRepeating(byte buttonToCheck, byte framesToDelay){
  return (frameCounter % framesToDelay == 0) && isAnyButtonRepeating(buttonToCheck);
}
bool Controller::isAnyButtonFrameDelayedRepeating(byte buttonToCheck, byte framesToDelay){
  return (frameCounter % framesToDelay == 0) && (currDelayedRepeating & buttonToCheck);
}

bool Controller::isButtonChordHeld(byte buttonMask) {
  if ((currHeld & buttonMask) != buttonMask) return false;
  // Fire on the frame the slowest-held button reaches the threshold.
  // All buttons must be >= threshold; the minimum being exactly threshold
  // ensures this fires once and only once.
  byte minTimer = 255;
  for (byte bit = 0; bit < 8; bit++) {
    if (buttonMask & (1 << bit)) {
      if (buttonTimers[bit] < minTimer) minTimer = buttonTimers[bit];
    }
  }
  return minTimer == chordHoldThreshold;
}


ControllerList::ControllerList(Controller* inController){
  controller = inController;
  numControls = 0;
}

void ControllerList::addControl(uint16_t inID, byte inButtons, ControlFunction inFunc, void* inArgs) {
  if (numControls < MAX_CONTROLS) {
      listPriorities[numControls] = CONTROL_GAME;
      listIDs[numControls] = inID;
      listControls[numControls] = inButtons;
      funcControls[numControls] = inFunc;
      funcArgs[numControls] = inArgs;
      numControls++;
  }
}

void ControllerList::addPriorityControl(uint16_t inID, byte inButtons, ControlFunction inFunc, void* inArgs) {
  if (numControls < MAX_CONTROLS) {
      listPriorities[numControls] = CONTROL_PERSISTENT;
      listIDs[numControls] = inID;
      listControls[numControls] = inButtons;
      funcControls[numControls] = inFunc;
      funcArgs[numControls] = inArgs;
      numControls++;
  }
}

void ControllerList::clearControls() {
    uint8_t n = 0;
    for (uint8_t i = 0; i < numControls; i++) {
        if (listPriorities[i] >= CONTROL_PERSISTENT) {
            listPriorities[n] = listPriorities[i];
            listIDs[n]        = listIDs[i];
            listControls[n]   = listControls[i];
            funcControls[n]   = funcControls[i];
            funcArgs[n]       = funcArgs[i];
            n++;
        }
    }
    numControls = n;
}

void ControllerList::runControls() {
    if (numModalControls > 0) {
        for (uint8_t i = 0; i < numModalControls; ++i) {
            if (controller->isIDMasked(modalListIDs[i], modalListControls[i], 1))
                modalFuncControls[i](modalFuncArgs[i]);
        }
    } else {
        for (byte controlIndex = 0; controlIndex < numControls; ++controlIndex) {
            if (controller->isIDMasked(listIDs[controlIndex], listControls[controlIndex], 1))
                funcControls[controlIndex](funcArgs[controlIndex]);
        }
    }
}

void ControllerList::addModalControl(uint16_t inID, byte inButtons,
                                     ControlFunction inFunc, void* inArgs) {
    if (numModalControls < MAX_MODAL_CONTROLS) {
        modalListIDs[numModalControls]      = inID;
        modalListControls[numModalControls] = inButtons;
        modalFuncControls[numModalControls] = inFunc;
        modalFuncArgs[numModalControls]     = inArgs;
        numModalControls++;
    }
}

void ControllerList::clearModalControls() { numModalControls = 0; }

byte ControllerList::getNumControls(){
  return numControls;
}


#endif
