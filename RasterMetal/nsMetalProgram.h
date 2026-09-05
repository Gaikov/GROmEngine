//
// Created by OpenCode
//
#pragma once

#include <Metal/Metal.h>
#include "nsMetalConfig.h"
#include "nsLib/matrix4.h"
#include "Engine/RenDevice.h"
#include <string>
#include <vector>
#include <map>

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
    float userParams[64];
};

class nsMetalProgram;

class nsMetalUniform : public IShaderUniform {
public:
    nsMetalUniform(nsMetalProgram *program, const char *name);

    void SetFloat(float value) override;
    void SetFloat3(const float value[3]) override;
    void SetFloat4(const float value[4]) override;

    void ResolveAndApply();

private:
    void Set(const float *value, int count);

    nsMetalProgram *_program;
    std::string     _name;
    float          *_fieldPtr = nullptr;
    float           _value[4] = {};
    int             _count    = 0;
};

class nsMetalProgram {
public:
    nsMetalProgram(id<MTLDevice> device);
    ~nsMetalProgram();

    bool Load(const char *vertexShaderPath, const char *fragmentShaderPath);
    void Unload();
    void Invalidate();

    id<MTLRenderPipelineState> GetPipelineState();
    id<MTLFunction> GetVertexFunction();
    id<MTLFunction> GetFragmentFunction();

    void SetProjView(const float *matrix);
    void SetModel(const float *matrix);
    void SetTextureMatrix(const float *matrix);
    void SetColor(const float *color);
    void SetAlphaCutoff(float cutoff);
    void SetHasTexture(bool hasTexture);
    void SetHasVertexColor(bool hasVertexColor);

    bool Bind(id<MTLRenderCommandEncoder> encoder);
    void UploadUniforms(id<MTLRenderCommandEncoder> encoder, uint frameIndex);

    IShaderUniform *GetUniform(const char *name);
    bool FindUniformOffset(const char *name, size_t &offset);
    MetalUniforms *GetUniforms() { return &_uniforms; }
    void ResolveUserUniforms();

private:
    id<MTLDevice>               _device            = nil;
    id<MTLRenderPipelineState>  _pipelineState     = nil;
    std::string                 _vertexShaderPath;
    std::string                 _fragmentShaderPath;
    std::vector<id<MTLBuffer>>  _uniformBuffers[kMetalInFlightFrameSlots];
    uint                        _lastFrameIndex    = ~0u;
    uint                        _uniformSlot       = 0;
    MetalUniforms               _uniforms          = {};
    nsMetalShaderLibrary       *_shaderLibrary     = nullptr;
    id<MTLFunction>             _vertexFunction    = nil;
    id<MTLFunction>             _fragmentFunction  = nil;

    std::map<std::string, size_t>       _uniformOffsets;
    std::map<std::string, IShaderUniform*> _userUniforms;

    bool EnsureUniformBuffer(uint frameSlot, uint uniformSlot);
    bool EnsureLoaded();
};
