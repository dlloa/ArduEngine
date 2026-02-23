#ifndef INTERFACES
#define INTERFACES
#include "interfaces.h"
//
Updateable::Updateable(FrameCounter* inFrameCounter) : _frameCounter(inFrameCounter) {}
void Updateable::update();

//
Renderable::Renderable(Arduboy2* inArduboy) : arduboy(inArduboy) {}
void Renderable::render();
//
#endif