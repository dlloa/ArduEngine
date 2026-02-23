#ifndef UPDATEABLE_MANAGER_H
#define UPDATEABLE_MANAGER_H

#include "../gameengine.h"
#include "../utilities/enumptrlistmap.h"

template <typename T>
class UpdateableManager : public Updateable {
public:
    UpdatePipeline* pipeline;
    // Pipeline priority level owned by this manager (typically PIPELINE_GAME).
    uint8_t gamePriority;
    StateMachine<T>* stateMachine;
    EnumPtrListMap<T, Updateable, 2> updateables;

    UpdateableManager(FrameCounter* fc, UpdatePipeline* pl, uint8_t priority, StateMachine<T>* sm)
        : Updateable(fc), pipeline(pl), gamePriority(priority), stateMachine(sm) {}

    void update() override {
        // Only rebuild the update list when a state transition is pending.
        if (!stateMachine->isTransitionFinished()) {
            pipeline->clear(gamePriority);
            for (uint8_t i = 0; i < updateables.size(stateMachine->getState()); i++) {
                if (updateables.has(stateMachine->getState(), i))
                    pipeline->add(gamePriority,
                        updateables.get(stateMachine->getState(), i));
            }
        }
    }
};

#endif
