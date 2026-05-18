//
// Created by OpenCode
//
#pragma once

#include <Metal/Metal.h>
#include "nsLib/matrix4.h"

class nsMetalShaderLibrary;

struct MetalUniforms {
    float projView[16];
    float model[16];
    float texMat[16];
    float alphaCutoff;
    int   hasTexture;
    float padding[2];
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
    void SetAlphaCutoff(float cutoff);
    void SetHasTexture(bool hasTexture);

    bool Bind(id<MTLRenderCommandEncoder> encoder);
    void UploadUniforms(id<MTLRenderCommandEncoder> encoder);

private:
    id<MTLDevice>               _device            = nil;
    id<MTLRenderPipelineState>  _pipelineState     = nil;
    id<MTLBuffer>               _uniformBuffer     = nil;
    MetalUniforms               _uniforms          = {};
    nsMetalShaderLibrary       *_shaderLibrary     = nullptr;
    id<MTLFunction>             _vertexFunction    = nil;
    id<MTLFunction>             _fragmentFunction  = nil;
};
