#ifndef PIPELINE_H
#define PIPELINE_H

#include <stdint.h>

// Priority convention: lower value = runs first.
// Update pipeline stages (run in order: PRE → SCENE → GAME):
static const uint8_t PIPELINE_PRE   = 0; // Global pre-update (e.g. MenuStateSetter)
static const uint8_t PIPELINE_SCENE = 1; // Scene orchestration (SceneManager)
static const uint8_t PIPELINE_GAME    = 2; // Per-state game objects (managed by UpdateableManager)
static const uint8_t PIPELINE_OVERLAY = 3; // Overlay layer — renders on top of PIPELINE_GAME

// Control priorities (used by ControllerList):
static const uint8_t CONTROL_GAME       = 0; // Cleared on every state transition
static const uint8_t CONTROL_PERSISTENT = 1; // Survives state transitions

template <typename T>
class PipelineBase {
public:
    struct Entry { uint8_t priority; T* ptr; };
    // Tunable: maximum entries per pipeline (UpdatePipeline and RenderPipeline).
    // Current minimum at 5: UpdatePipeline uses max 3, RenderPipeline uses max 4.
    // Increase if adding more pipeline stages or overlay layers.
    static const uint8_t MAX_ITEMS = 5;
    Entry entries[MAX_ITEMS];
    uint8_t count = 0;

    // Insert keeping entries sorted by ascending priority.
    void add(uint8_t priority, T* ptr) {
        if (count >= MAX_ITEMS) return;
        uint8_t pos = count;
        while (pos > 0 && entries[pos - 1].priority > priority) {
            entries[pos] = entries[pos - 1];
            pos--;
        }
        entries[pos] = {priority, ptr};
        count++;
    }

    // Remove all entries at the given priority level.
    void clear(uint8_t priority) {
        uint8_t n = 0;
        for (uint8_t i = 0; i < count; i++)
            if (entries[i].priority != priority) entries[n++] = entries[i];
        count = n;
    }
};

#endif
