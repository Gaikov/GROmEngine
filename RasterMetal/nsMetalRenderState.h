//
// Created by OpenCode
//
#pragma once

#include "Engine/RenDevice.h"
#import <Metal/Metal.h>

class nsMetalRenderState : public IRenState {
public:
    nsMetalRenderState(id<MTLRenderPipelineState> pipelineState);
    ~nsMetalRenderState() override = default;

    bool IsAlphaTest() override { return false; }
    bool IsAlphaBlend() override { return false; }

    id<MTLRenderPipelineState> GetPipelineState() const { return _pipelineState; }

private:
    id<MTLRenderPipelineState> _pipelineState;
};
