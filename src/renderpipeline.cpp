#include "renderpipeline.h"

void RenderPipeline::renderAll() {
    for (uint8_t i = 0; i < count; i++) {
        entries[i].ptr->render();
    }
}
