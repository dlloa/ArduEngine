#ifndef INTERFACES_H
#define INTERFACES_H

#include <Arduboy2.h>

class FrameCounter;

class Updateable {
public:
    FrameCounter* _frameCounter;
    Updateable(FrameCounter* inFrameCounter);
    virtual void update() = 0;
};

class Renderable {
public:
    Arduboy2* arduboy;
    Renderable(Arduboy2* inArduboy);
    virtual void render() = 0;
};

#endif
