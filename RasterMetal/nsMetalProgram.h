//
// Created by OpenCode
//
#pragma once

#include <Metal/Metal.h>
#include "nsMetalConfig.h"
#include "nsLib/matrix4.h"
#include <vector>

class nsMetalShaderLibrary;

struct MetalUniforms {
    float projView[16];
    float model[16];
    float texMat[16];
    float color[4];
    float alphaCutoff;
    int   hasTexture;
    int   hasVertexColor;
    float padding;
};

class nsMetalProgram {
public:
    nsMetalProgram(id<MTLDevice> device);
    ~nsMetalProgram();

    bool Load(const char *vertexShaderPath, const char *fragmentShaderPath);
    void Unload();

    id<MTLRenderPipelineState> GetPipelineState() const { return _pipelineState; }
    id<MTLFunction> GetVertexFunction() const { return _vertexFunction; }
    id<MTLFunction> GetFragmentFunction() const { return _fragmentFunction; }

    void SetProjView(const float *matrix);
    void SetModel(const float *matrix);
    void SetTextureMatrix(const float *matrix);
    void SetColor(const float *color);
    void SetAlphaCutoff(float cutoff);
    void SetHasTexture(bool hasTexture);
    void SetHasVertexColor(bool hasVertexColor);

    bool Bind(id<MTLRenderCommandEncoder> encoder);
    void UploadUniforms(id<MTLRenderCommandEncoder> encoder, uint frameIndex);

private:
    id<MTLDevice>               _device            = nil;
    id<MTLRenderPipelineState>  _pipelineState     = nil;
    std::vector<id<MTLBuffer>>  _uniformBuffers[kMetalInFlightFrameSlots];
    uint                        _lastFrameIndex    = ~0u;
    uint                        _uniformSlot       = 0;
    MetalUniforms               _uniforms          = {};
    nsMetalShaderLibrary       *_shaderLibrary     = nullptr;
    id<MTLFunction>             _vertexFunction    = nil;
    id<MTLFunction>             _fragmentFunction  = nil;

    bool EnsureUniformBuffer(uint frameSlot, uint uniformSlot);
};
