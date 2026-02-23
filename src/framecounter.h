#ifndef FRAME_COUNTER_H
#define FRAME_COUNTER_H

#include <Arduboy2.h>

class FrameCounter {
public:
    // Free-running byte counter, incremented each frame and wrapping at 255.
    // Used for interval/timing checks throughout the engine.
    byte _frame = 0;
    FrameCounter();
    void Increment();
    byte GetFrame();
};

#endif
