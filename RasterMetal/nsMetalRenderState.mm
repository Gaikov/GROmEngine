//
// Created by OpenCode
//

#include "nsMetalRenderState.h"

nsMetalRenderState::nsMetalRenderState(id<MTLRenderPipelineState> pipelineState)
    : _pipelineState(pipelineState) {}
