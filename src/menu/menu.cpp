#ifndef MENU
#define MENU

#include "menu/menu.h"
#include "utilities/utilities.h"

Menu::Menu(Arduboy2* arduboy, ControllerList* inControllerList, byte inMaxSelection, const char* const* menuItems)
  : Controllable(inControllerList), Renderable(arduboy), maxSelection(inMaxSelection), items(menuItems) {}

void Menu::takeControl(){
    addControl(BUTTON_JUST_PRESSED | BUTTON_FRAME_DELAYED_REPEATING, UP_BUTTON, &Menu::MOVE_UP, this);
    addControl(BUTTON_JUST_PRESSED | BUTTON_FRAME_DELAYED_REPEATING, DOWN_BUTTON, &Menu::MOVE_DOWN, this);
    addControl(BUTTON_JUST_PRESSED, B_BUTTON, &Menu::MAKE_SELECTION, this);
    addControl(BUTTON_JUST_PRESSED, A_BUTTON, &Menu::UNSELECT, this);
}

void Menu::loseControl(){

}

void Menu::render(){

  int vertSpacing = 10;
  for(int itemIndex = 0; itemIndex < maxSelection; itemIndex++){

    char buffer[20];
    strcpy_P(buffer, (char*)pgm_read_word(&(items[itemIndex])));

    arduboy->setCursor(xPos, yPos + (itemIndex * vertSpacing));
    if (itemIndex == currentSelection) {
        arduboy->print(madeSelection ? '>' : '-');
    }
    arduboy->print(buffer);

  }

}

void Menu::MoveUp(){
  if( !madeSelection )
    WrapValue( &(currentSelection), maxSelection, false);
}
void Menu::MoveDown(){
  if( !madeSelection )
    WrapValue( &(currentSelection), maxSelection, true);
}
void Menu::Select(){
  madeSelection = true;
}
void Menu::Unselect(){
  madeSelection = false;
}

static void Menu::MOVE_UP(void* data){
  Menu* menu = static_cast<Menu*>(data);
  menu->MoveUp();
}
static void Menu::MOVE_DOWN(void* data ){
  Menu* menu = static_cast<Menu*>(data);
  menu->MoveDown();
}
static void Menu::MAKE_SELECTION(void* data ){
  Menu* menu = static_cast<Menu*>(data);
  menu->Select();
}
static void Menu::UNSELECT(void* data ){
  Menu* menu = static_cast<Menu*>(data);
  menu->Unselect();
}

void Menu::setPosition(byte inXPos, byte inYPos){
  xPos = inXPos;
  yPos = inYPos;
}

byte Menu::getSelection(){
  return currentSelection;
}
byte Menu::getMadeSelection(){
  return madeSelection;
}

#endif