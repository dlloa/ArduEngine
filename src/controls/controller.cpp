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

	//Updating button timers
	for( byte bit = 0; bit < 8; bit++ )
  {
	 //If button held, increase timer
  	if( currHeld & ( 1 << bit ) ){
    	  //Update regular button timer up to max frames
    	  if( buttonTimers[bit] < repeatDelayFrames){
    	    buttonTimers[bit]++;
    	  }
    	  //If regular timer has been reached
    	  if( buttonTimers[bit] >= repeatDelayFrames ){
    	    //Increment delayed input repeat timer, wrap back to 0 after a frame.
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

    //Check button timers, update values for currently repeating buttons
    if( buttonTimers[bit] >= repeatDelayFrames ){
      //Reached frame count, set to currently repeating
      currRepeating |= 1 << bit;
      //Always 
    }
    else{
      //Clear currRepeating if we haven't reached that count/0
      byte mask = 0b00000001 << bit;
      mask = ~mask;
      currRepeating &= mask;
    }


    //Same check as above, but for delayed input repeating value
    if( repeatDelayButtonTimers[bit] == repeatDelayFrames - 1 ){
      //Reached frame count, set to currently repeating
      currDelayedRepeating |= 1 << bit;
      //Always 
    }
    else{
      //Clear currRepeating if we haven't reached that count/0
      byte mask = 0b00000001 << bit;
      mask = ~mask;
      currDelayedRepeating &= mask;
    }

  }


}

bool Controller::isID(ControllerID inID, byte buttonToCheck, byte inFrames = 1){
  bool toReturn = false;
  switch(inID){
    case BUTTON_JUST_PRESSED: //isButtonJustPressed
      toReturn = isButtonJustPressed(buttonToCheck);
      break;
    case BUTTON_JUST_RELEASED: //isButtonJustReleased
      toReturn = isButtonJustReleased(buttonToCheck);
      break;
    case BUTTON_HELD: //isButtonHeld
      toReturn = isButtonHeld(buttonToCheck);
      break;
    case BUTTON_REPEATING: //isButtonRepeating
      toReturn = isButtonRepeating(buttonToCheck);
      break;
    case BUTTON_DELAYED_REPEATING: //isButtonDelayedRepeating
      toReturn = isButtonDelayedRepeating(buttonToCheck, inFrames);
      break;
    case ANY_BUTTON_JUST_PRESSED: //isAnyButtonJustPressed
      toReturn = isAnyButtonJustPressed(buttonToCheck);
      break;
    case ANY_BUTTON_REPEATING: //isAnyButtonRepeating
      toReturn = isAnyButtonRepeating(buttonToCheck);
      break;
    case ANY_BUTTON_DELAYED_REPEATING: //isAnyButtonDelayedRepeating
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

String Controller::debugPrint(){
  String string = String(prevButtons, BIN) + String("\n");
  string += String(currButtons, BIN) + String("\n");
  string += String(currHeld, BIN) + String("\n");
  string += String(currReleased, BIN) + String("\n");
  string += String(currRepeating, BIN) + String("\n");
  string += String(buttonMemoryCurrentFrames) + String("\n");
  // string += String(memoryPrint()) + String("\n");
  return string;
}

String Controller::quickPrint(){
  String string = String(currButtons, BIN);
  return string;
}

//
ControllerMemory::ControllerMemory(Arduboy2* inArduboy, FrameCounter* inFrameCounter, Controller* inController) 
  : Renderable(inArduboy), Updateable(inFrameCounter), controller(inController){

  }

void ControllerMemory::render() {

}

void ControllerMemory::update() {
  if( controller -> prevButtons != controller -> currButtons ){

    //If anything is pressed
    if( controller -> currButtons != 0 ){
      moveButtonMemory();
      addButtonMemory(controller -> currButtons);
    }

    //Previous held button combo frame time clear
    buttonMemoryCurrentFrames = 0;
  }

  //8 Bit Value for button memory frames, hold at max
  if( buttonMemoryCurrentFrames < 255 ){
    buttonMemoryCurrentFrames += 1;
  }

  //No buttons held for buttonMemoryFrames will clear memory
  if( controller -> currButtons == 0 ){
    if( buttonMemoryCurrentFrames == buttonMemoryFrames ){
      clearButtonMemory();
    }
  }
}

void ControllerMemory::addButtonMemory(byte inMemory){
  memoryCleared = false;
  buttonMemory[0] = inMemory;
}

void ControllerMemory::clearButtonMemory(){
  if( !memoryCleared ){
    for( byte memoryIndex=0; memoryIndex < buttonMemorySize; memoryIndex++){
      buttonMemory[memoryIndex] = 0;
    }
  }
  memoryCleared = true;
}

void ControllerMemory::moveButtonMemory(){
  byte temp = buttonMemory[0];
  buttonMemory[0] = 0;
  for( byte memoryIndex=0; memoryIndex < buttonMemorySize; memoryIndex++){
    byte newtemp;
    if( memoryIndex + 1 < buttonMemorySize){
      newtemp = buttonMemory[memoryIndex + 1];
      buttonMemory[memoryIndex + 1] = temp;
      temp = newtemp;
    }
  }
}

String ControllerMemory::memoryPrint(){
  String toReturn = "";
  for( byte memoryIndex=0; memoryIndex < buttonMemorySize; memoryIndex++){
    if( buttonMemory[memoryIndex] != 0 ){
      toReturn += String(buttonMemory[memoryIndex]) + String(",");
    }
  }
  return toReturn;
}

//
ControllerList::ControllerList(Controller* inController){
  controller = inController;
  numControls = 0;
}

void ControllerList::addControl(uint16_t inID, byte inButtons, ControlFunction inFunc, void* inArgs) {
  if (numControls < MAX_CONTROLS) {
      listIDs[numControls] = inID;
      listControls[numControls] = inButtons;
      funcControls[numControls] = inFunc;
      funcArgs[numControls] = inArgs;
      numControls++;
  }
}

void ControllerList::addPriorityControl(uint16_t inID, byte inButtons, ControlFunction inFunc, void* inArgs) {
  addControl(inID, inButtons, inFunc, inArgs);
  priorityControls++;
}

void ControllerList::clearControls() {
    numControls = priorityControls; // Simply reset the count, no need to delete memory
}

void ControllerList::runControls() {
  for (byte controlIndex = 0; controlIndex < numControls; ++controlIndex) {
    if ( controller -> isIDMasked(listIDs[controlIndex], listControls[controlIndex], 1)){
        funcControls[controlIndex](funcArgs[controlIndex]);
    }
  }
}

byte ControllerList::getNumControls(){
  return numControls;
}

String ControllerList::debugOutput(){
  String string = String(getNumControls()) + String(":") + MAX_CONTROLS + String("\n");
  return string;
}
//

#endif