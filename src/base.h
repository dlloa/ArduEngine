#ifndef BASE_H
#define BASE_H

#include <Arduboy2.h>

class FrameCounter;

// Per-frame logic object. update() is called each frame by UpdatePipeline.
class Updateable {
public:
    FrameCounter* _frameCounter;
    PGM_P dbgName = nullptr;  // Optional PROGMEM name shown in debug menu.
    Updateable(FrameCounter* inFrameCounter) : _frameCounter(inFrameCounter) {}
    virtual void update() = 0;
};

// Per-frame draw object. render() is called each frame by RenderPipeline.
class Renderable {
public:
    Arduboy2* arduboy;
    PGM_P dbgName = nullptr;  // Optional PROGMEM name shown in debug menu.
    Renderable(Arduboy2* inArduboy) : arduboy(inArduboy) {}
    virtual void render() = 0;
};

#endif
