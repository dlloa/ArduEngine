#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "../gameengine.h"
#include "renderablemanager.h"
#include "updateablemanager.h"
#include "statemachineupdateable.h"

// Forward declaration to avoid circular include with controllablemanager.h
template <typename T> class ControllableManager;

// Orchestrates all four transition managers each frame. sm->update() runs last
// so that renderable/updateable/control lists are rebuilt before the state
// machine marks the transition as finished.
template <typename T>
class SceneManager : public Updateable {
public:
    RenderableManager<T>*      renderableMgr;
    UpdateableManager<T>*      updateableMgr;
    ControllableManager<T>*    controllerMgr;
    StateMachineUpdateable<T>* sm;

    SceneManager(FrameCounter* fc,
                 RenderableManager<T>* rm,
                 UpdateableManager<T>* um,
                 ControllableManager<T>* cm,
                 StateMachineUpdateable<T>* s)
        : Updateable(fc), renderableMgr(rm), updateableMgr(um), controllerMgr(cm), sm(s) {}

    void update() override {
        renderableMgr->update();
        updateableMgr->update();
        controllerMgr->update();
        sm->update();
    }
};

#endif
