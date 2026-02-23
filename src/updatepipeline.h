#ifndef UPDATE_PIPELINE_H
#define UPDATE_PIPELINE_H

#include "pipeline.h"
#include "base.h"

// Calls update() on all registered Updateable entries in priority order.
class UpdatePipeline : public PipelineBase<Updateable> {
public:
    void updateAll();
};

#endif
