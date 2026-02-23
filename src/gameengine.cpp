#ifndef GAME_ENGINE
#define GAME_ENGINE
#include "gameengine.h"
#include "controls/controller.h"
#include "utilities/utilities.h"

//
void SystemInfo::initMemUsage(){
    minRam = freeMemory();
}

void SystemInfo::updateMemUsage(){
  ram = freeMemory();
  if (ram < minRam) {
    minRam = ram;
  }
}

int16_t SystemInfo::freeMemory() {
  char top;
  if (__brkval == 0) {
    return ((int)&top) - ((int)&__heap_start);
  } else {
    return ((int)&top) - ((int)__brkval);
  }
}

void SystemInfo::StartFrame(){
    startTime = micros();
}
unsigned long SystemInfo::FrameTime(){
    return micros() - startTime;
}


//

RenderableSystemInfo::RenderableSystemInfo(Arduboy2* inArduboy, FrameCounter* inFrameCounter) : Renderable(inArduboy), Updateable(inFrameCounter){

}

void RenderableSystemInfo::render(){
  // sysInfo.updateMemUsage();

  arduboy -> setCursor(0, 32);
  arduboy -> print(sysInfo.ram);
  arduboy -> setCursor(0, 40);
  arduboy -> print(sysInfo.minRam);

  arduboy -> setCursor(0, 48);
  arduboy -> print("T:");
  arduboy -> print(sysInfo.FrameTime());
}

void RenderableSystemInfo::update(){
  sysInfo.StartFrame();
  sysInfo.updateMemUsage();
}

//
FrameCounter::FrameCounter(){}

void FrameCounter::Increment(){
    _frame++;
}
byte FrameCounter::GetFrame(){
    return _frame;
}

//
Controllable::Controllable(ControllerList* inControllerList) : controllerList(inControllerList) {}

void Controllable::addControl(uint16_t inID, byte inControl, void (*inFunc)(void*), void* inArgs) {
    controllerList->addControl(inID, inControl, inFunc, inArgs);
}

void Controllable::addPriorityControl(uint16_t inID, byte inControl, void (*inFunc)(void*), void* inArgs){
  controllerList->addPriorityControl(inID, inControl, inFunc, inArgs);
}

// void Controllable::takeControl(){};

void Controllable::clearControls() {
    controllerList->clearControls();
}

// Menu

// Menu::Menu(Arduboy2* arduboy, ControllerList* inControllerList, byte inMaxSelection, const char* const* menuItems)
//   : Controllable(inControllerList), Renderable(arduboy), maxSelection(inMaxSelection), items(menuItems) {}

// void Menu::takeControl(){
//     addControl(BUTTON_JUST_PRESSED | BUTTON_FRAME_DELAYED_REPEATING, UP_BUTTON, &Menu::MOVE_UP, this);
//     addControl(BUTTON_JUST_PRESSED | BUTTON_FRAME_DELAYED_REPEATING, DOWN_BUTTON, &Menu::MOVE_DOWN, this);
//     addControl(BUTTON_JUST_PRESSED, B_BUTTON, &Menu::MAKE_SELECTION, this);
//     addControl(BUTTON_JUST_PRESSED, A_BUTTON, &Menu::UNSELECT, this);
// }

// void Menu::render(){

//   int vertSpacing = 10;
//   for(int itemIndex = 0; itemIndex < maxSelection; itemIndex++){

//     char buffer[20];
//     strcpy_P(buffer, (char*)pgm_read_word(&(items[itemIndex])));
//     String toPrint = itemIndex == currentSelection ? ( madeSelection ? (">") : ("-")) + String(buffer) : String(buffer);

//     arduboy->setCursor(xPos, yPos + (itemIndex * vertSpacing));
//     arduboy->print(toPrint);
    
//   }

// }

// static void Menu::MOVE_UP(void* data){
//   Menu* menu = static_cast<Menu*>(data);
//   if( !menu->madeSelection )
//     WrapValue( &(menu -> currentSelection), menu -> maxSelection, false);
// }
// static void Menu::MOVE_DOWN(void* data ){
//   Menu* menu = static_cast<Menu*>(data);
//   if( !menu->madeSelection )
//     WrapValue( &(menu -> currentSelection), menu -> maxSelection, true);
// }
// static void Menu::MAKE_SELECTION(void* data ){
//   Menu* menu = static_cast<Menu*>(data);
//   menu -> madeSelection = true;
// }
// static void Menu::UNSELECT(void* data ){
//   Menu* menu = static_cast<Menu*>(data);
//   menu -> madeSelection = false;
// }

// void Menu::setPosition(byte inXPos, byte inYPos){
//   xPos = inXPos;
//   yPos = inYPos;
// }

// byte Menu::getSelection(){
//   return currentSelection;
// }
// byte Menu::getMadeSelection(){
//   return madeSelection;
// }

//

void RenderList::addRenderable(Renderable* inRenderable) {
    if (nNumRenderable < MAX_RENDERABLES) {
        aRenderables[nNumRenderable] = inRenderable;
        nNumRenderable++;
    }
}

void RenderList::clearRenderables(){
  nNumRenderable = 0;
}

void RenderList::renderAll() {
    for (int i = 0; i < nNumRenderable; i++) {
        aRenderables[i]->render();
    }
}

//ControllableStateMachine
//

//State Machine Stuff

// template <typename StateEnum>
// StateMachine<StateEnum>::StateMachine(StateEnum initialState)
// // : Controllable(inControllerList), Updateable(inFrameCounter), currentState(initialState), bTransitionFinished(true){
// : currentState(initialState), bTransitionFinished(true){

// }


// template <typename StateEnum>
// void StateMachine<StateEnum>::setState(StateEnum newState) {
//     if (isValidState(newState)) {
//         currentState = newState;
//         bTransitionFinished = false; // Transition started
//     }
// }


// template <typename StateEnum>
// StateEnum StateMachine<StateEnum>::getState() const {
//     return currentState;
// }


// template <typename StateEnum>
// void StateMachine<StateEnum>::nextState() {
//     int nextStateValue = static_cast<int>(currentState) + 1;
//     if (nextStateValue > static_cast<int>(StateEnum::STATE_MAX)) {
//         nextStateValue = static_cast<int>(StateEnum::STATE_MIN); // Wrap around to the first state
//     }
//     currentState = static_cast<StateEnum>(nextStateValue);
//     bTransitionFinished = false; // Transition started
// }


// template <typename StateEnum>
// void StateMachine<StateEnum>::previousState() {
//     int prevStateValue = static_cast<int>(currentState) - 1;
//     if (prevStateValue < static_cast<int>(StateEnum::STATE_MIN)) {
//         prevStateValue = static_cast<int>(StateEnum::STATE_MAX); // Wrap around to the last state
//     }
//     currentState = static_cast<StateEnum>(prevStateValue);
//     bTransitionFinished = false; // Transition started
// }


// template <typename StateEnum>
// bool StateMachine<StateEnum>::isState(StateEnum state) const {
//     return currentState == state;
// }

// template <typename StateEnum>
// bool StateMachine<StateEnum>::isTransitionFinished() const {
//     return bTransitionFinished;
// }

// template <typename StateEnum>
// void StateMachine<StateEnum>::markTransitionFinished() {
//     bTransitionFinished = true;
// }

// template <typename StateEnum>
// static void StateMachine<StateEnum>::nextStateWrapper(void* data) {
//     StateMachine* stateMachine = static_cast<StateMachine*>(data);
//     stateMachine->nextState();
// }

// template <typename StateEnum>
// static void StateMachine<StateEnum>::previousStateWrapper(void* data) {
//     StateMachine* stateMachine = static_cast<StateMachine*>(data);
//     stateMachine->previousState();
// }

// template <typename StateEnum>
// bool StateMachine<StateEnum>::isValidState(StateEnum state) const {
//     return (static_cast<int>(state) >= static_cast<int>(StateEnum::STATE_MIN) &&
//             static_cast<int>(state) <= static_cast<int>(StateEnum::STATE_MAX));
// }

//

// template <typename T>
// ControllableManager<T>::ControllableManager(T initialState) {

// }

//

void Animator::setPosition(int inX, int inY){
  posX = inX;
  posY = inY;
}

Animator::Animator(Arduboy2* inArduboy, FrameCounter* inFrameCounter, const unsigned char** inSprite, int inSize, int inFrames, int inFrameRate) 
: Renderable(inArduboy), Updateable(inFrameCounter)
{
  size = inSize;
  sprite = inSprite;
  framerate = inFrameRate;
  frames = inFrames;
}

void Animator::setSprite(const unsigned char** newSprite, int newFrames) {
  sprite = newSprite;
  frames = newFrames;
  currentframe = 0;
  framecounter = 0;
}

void Animator::startAnimation() {
  bAnimating = true;
  currentframe = 0;
  framecounter = 0;
}

void Animator::stopAnimation() {
  bAnimating = false;
  currentframe = 0;
  framecounter = 0;
}

void Animator::update() {
  if( bAnimating ){
    framecounter++;
    if( framecounter >= framerate ){
      framecounter = 0;
      currentframe++;
      if( currentframe >= frames ){
        currentframe = 0;
      }
    }
  }
}

void Animator::render() {
  arduboy -> drawBitmap(posX, posY, sprite[currentframe], size, size, WHITE);
}


#endif