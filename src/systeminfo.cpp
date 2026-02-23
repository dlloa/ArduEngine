#include "systeminfo.h"

void SystemInfo::initMemUsage() {
    minRam = freeMemory();
}

void SystemInfo::updateMemUsage() {
    ram = freeMemory();
    if (ram < minRam) {
        minRam = ram;
    }
}

int16_t SystemInfo::freeMemory() {
    // Stack pointer minus the top of the heap gives free RAM.
    // __brkval is 0 when the heap has never been used; fall back to __heap_start.
    char top;
    if (__brkval == 0) {
        return ((int)&top) - ((int)&__heap_start);
    } else {
        return ((int)&top) - ((int)__brkval);
    }
}

void SystemInfo::StartFrame() {
    startTime = micros();
}

unsigned long SystemInfo::FrameTime() {
    return micros() - startTime;
}

RenderableSystemInfo::RenderableSystemInfo(Arduboy2* inArduboy, FrameCounter* inFrameCounter)
    : Renderable(inArduboy), Updateable(inFrameCounter) {}

void RenderableSystemInfo::render() {
    arduboy->setCursor(0, 32);
    arduboy->print(sysInfo.ram);
    arduboy->setCursor(0, 40);
    arduboy->print(sysInfo.minRam);
    arduboy->setCursor(0, 48);
    arduboy->print("T:");
    arduboy->print(sysInfo.FrameTime());
}

void RenderableSystemInfo::update() {
    sysInfo.StartFrame();
    sysInfo.updateMemUsage();
}
