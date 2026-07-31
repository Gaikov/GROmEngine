//
// Created by OpenCode
//
#pragma once

#include "Engine/RenDevice.h"
#include "nsMetalProgram.h"
#include "nsMetalProgramsCache.h"
#include "nsLib/StrTools.h"

#include <array>

#import <Metal/Metal.h>

class nsMetalRenderState : public IRenState {
public:
    nsMetalRenderState(id<MTLDevice> device, nsMetalProgramsCache &programs);
    ~nsMetalRenderState() override = default;

    bool Load(const char *fileName);
    bool InitDefault();
    void Invalidate();
    const char* GetPath() const { return _fileName; }

    bool IsAlphaTest() override { return _alphaTest; }
    bool IsAlphaBlend() override { return _alphaBlend; }
    float GetAlphaCutoff() const { return _alphaRef; }
    bool IsDepthEnabled() const { return _zEnable; }
    bool IsDepthWriteEnabled() const { return _zEnable && _zWrite; }
    void SetColorWriteMask(MTLColorWriteMask mask);

    void Apply(id<MTLRenderCommandEncoder> encoder,
               nsMetalProgramsCache &programs,
               nsMetalRenderState *prev);

    nsMetalProgram* GetProgram() const { return _program; }

private:
    nsString        _fileName;
    nsMetalProgram *_program = nullptr;
    nsMetalProgramsCache &_programs;
    id<MTLDevice> _device = nil;

    static constexpr size_t COLOR_WRITE_MASK_VARIANTS = 16;

    std::array<id<MTLRenderPipelineState>, COLOR_WRITE_MASK_VARIANTS> _pipelineStates{};
    id<MTLDepthStencilState>   _depthStencilState = nil;
    id<MTLSamplerState>        _samplerState = nil;

    bool    _zEnable        = true;
    bool    _zWrite         = true;
    bool    _alphaTest      = false;
    float   _alphaRef       = 0;
    bool    _alphaBlend     = false;
    MTLBlendFactor _srcBlend   = MTLBlendFactorOne;
    MTLBlendFactor _dstBlend   = MTLBlendFactorZero;
    MTLBlendFactor _srcAlphaBlend = MTLBlendFactorOne;
    MTLBlendFactor _dstAlphaBlend = MTLBlendFactorOne;
    bool    _cullMode       = true;
    MTLColorWriteMask _colorWriteMask = MTLColorWriteMaskAll;
    MTLSamplerAddressMode _texCoordU = MTLSamplerAddressModeRepeat;
    MTLSamplerAddressMode _texCoordV = MTLSamplerAddressModeRepeat;

    bool Parse(class script_state_t *ss);
    bool EnsureResources();
    bool CreatePipelineState(MTLColorWriteMask mask);
    bool CreateCommonStates();
    id<MTLRenderPipelineState> GetPipelineState() const;
};
