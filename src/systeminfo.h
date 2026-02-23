#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include "base.h"
#include "framecounter.h"

// AVR linker symbols for the heap region boundary.
extern unsigned int __heap_start;
extern void *__brkval;

// Tracks free RAM (current and minimum seen) and per-frame elapsed time.
// Used by RenderableSystemInfo to display debug stats on screen.
class SystemInfo {
public:
    int16_t ram;
    int16_t minRam;
    unsigned long startTime;
    void initMemUsage();
    void updateMemUsage();
    int16_t freeMemory();
    void StartFrame();
    unsigned long FrameTime();
};

// Renderable/Updateable wrapper that displays free-RAM and frame-time on screen.
class RenderableSystemInfo : public Renderable, public Updateable {
public:
    SystemInfo sysInfo;
    bool bDebugOn = true;
    RenderableSystemInfo(Arduboy2* inArduboy, FrameCounter* inFrameCounter);
    void render() override;
    void update() override;
};

#endif
