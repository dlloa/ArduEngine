#ifndef RENDER_PIPELINE_H
#define RENDER_PIPELINE_H

#include "pipeline.h"
#include "base.h"

// Calls render() on all registered Renderable entries in priority order.
class RenderPipeline : public PipelineBase<Renderable> {
public:
    void renderAll();
};

#endif
