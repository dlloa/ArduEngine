#include "framecounter.h"

FrameCounter::FrameCounter() {}

void FrameCounter::Increment() {
    _frame++;
}

byte FrameCounter::GetFrame() {
    return _frame;
}
