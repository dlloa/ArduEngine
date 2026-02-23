#ifndef DEBUG_OVERLAY_H
#define DEBUG_OVERLAY_H
#include "../base.h"
#include "../framecounter.h"
#include "../systeminfo.h"
#include "../updatepipeline.h"
#include "../renderpipeline.h"
#include "../managers/controller.h"

class DebugOverlay : public Renderable, public Updateable {
public:
    SystemInfo sysInfo;
    bool enabled = false;
    UpdatePipeline* updatePipeline;
    RenderPipeline* renderPipeline;
    ControllerList* controllerList;

    DebugOverlay(Arduboy2* ab, FrameCounter* fc,
                 UpdatePipeline* up, RenderPipeline* rp, ControllerList* cl)
        : Renderable(ab), Updateable(fc),
          updatePipeline(up), renderPipeline(rp), controllerList(cl) {}

    void update() override {
        sysInfo.StartFrame();
        sysInfo.updateMemUsage();
    }

    void render() override {
        if (!enabled) return;
        arduboy->setCursor(0, 48);
        arduboy->print(F("R:"));  arduboy->print(sysInfo.ram);
        arduboy->print(F("/"));   arduboy->print(sysInfo.minRam);
        arduboy->print(F(" T:")); arduboy->print(sysInfo.FrameTime());
        arduboy->setCursor(0, 56);
        arduboy->print(F("U:"));  arduboy->print(updatePipeline->count);
        arduboy->print(F(" R:")); arduboy->print(renderPipeline->count);
        arduboy->print(F(" C:")); arduboy->print(controllerList->numControls);
    }
};
#endif
