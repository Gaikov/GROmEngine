//
// Created by OpenCode
//
#pragma once

#include "Engine/RenDevice.h"
#include "nsMetalProgram.h"
#include "nsMetalProgramsCache.h"
#include "nsLib/StrTools.h"

#import <Metal/Metal.h>

class nsMetalRenderState : public IRenState {
public:
    nsMetalRenderState(id<MTLDevice> device, nsMetalProgramsCache &programs);
    ~nsMetalRenderState() override = default;

    bool Load(const char *fileName);
    const char* GetPath() const { return _fileName; }

    bool IsAlphaTest() override { return _alphaTest; }
    bool IsAlphaBlend() override { return _alphaBlend; }
    float GetAlphaCutoff() const { return _alphaRef; }

    void Apply(id<MTLRenderCommandEncoder> encoder,
               nsMetalProgramsCache &programs,
               nsMetalRenderState *prev);

    nsMetalProgram* GetProgram() const { return _program; }

private:
    nsString        _fileName;
    nsMetalProgram *_program = nullptr;
    nsMetalProgramsCache &_programs;
    id<MTLDevice> _device = nil;

    id<MTLRenderPipelineState> _pipelineState = nil;
    id<MTLDepthStencilState>   _depthStencilState = nil;

    bool    _zEnable        = true;
    bool    _zWrite         = true;
    bool    _alphaTest      = false;
    float   _alphaRef       = 0;
    bool    _alphaBlend     = false;
    MTLBlendFactor _srcBlend   = MTLBlendFactorOne;
    MTLBlendFactor _dstBlend   = MTLBlendFactorZero;
    bool    _cullMode       = true;

    bool Parse(class script_state_t *ss);
    bool CreatePipeline();
};
