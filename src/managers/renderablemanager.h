#ifndef RENDERABLE_MANAGER_H
#define RENDERABLE_MANAGER_H

#include "../gameengine.h"
#include "../utilities/enumptrlistmap.h"

template <typename T>
class RenderableManager : public Updateable {
public:
    RenderPipeline* renderPipeline;
    StateMachine<T>* stateMachine;
    // Max 2 Renderable* entries per state.
    EnumPtrListMap<T, Renderable, 2> renderables;
    RenderableManager(FrameCounter* fc, RenderPipeline* rp, StateMachine<T>* stateMachine) : Updateable(fc), renderPipeline(rp), stateMachine(stateMachine) {}

    void update() override {
        // Only rebuild the render list when a state transition is pending.
        if (!(stateMachine->isTransitionFinished())) {
            renderPipeline->clear(PIPELINE_GAME);
            for (uint8_t i = 0; i < renderables.size(stateMachine->getState()); i++) {
                if (renderables.has(stateMachine->getState(), i)) {
                    renderPipeline->add(PIPELINE_GAME, renderables.get(stateMachine->getState(), i));
                }
            }
        }
    }
};

#endif
