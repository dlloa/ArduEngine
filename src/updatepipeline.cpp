#include "updatepipeline.h"

void UpdatePipeline::updateAll() {
    for (uint8_t i = 0; i < count; i++) {
        entries[i].ptr->update();
    }
}
