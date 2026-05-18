//
// Created by OpenCode
//
#pragma once

#include "nsMetalProgram.h"
#include <map>
#include <string>

class nsMetalProgramsCache {
public:
    nsMetalProgramsCache(id<MTLDevice> device) : _device(device) {}
    ~nsMetalProgramsCache();

    bool Init();
    void Release();
    void Invalidate();

    nsMetalProgram* GetProgram(const char *vertexShaderPath, const char *fragmentShaderPath);

    void Bind(nsMetalProgram *program, bool force);
    void SetTextureBound(bool hasBound);
    void SetTextureMatrix(const nsMatrix &m);
    void SetAlphaCutoff(float alphaRef);
    void SetProjViewMatrix(const float *m);
    void SetModelMatrix(const float *m);

    nsMetalProgram* GetDefaultProgram() const { return _defaultProgram; }

    static constexpr auto DEFAULT_VERTEX_SHADER = "default/rs/metal/default.metal";
    static constexpr auto DEFAULT_FRAGMENT_SHADER = "default/rs/metal/default.metal";

private:
    id<MTLDevice>       _device            = nil;
    nsMetalProgram     *_defaultProgram    = nullptr;
    nsMetalProgram     *_currentProgram    = nullptr;
    std::map<std::string, nsMetalProgram*> _cache;

    bool _textureBound  = false;
    nsMatrix _textureMatrix;
    float _alphaCutoff  = 0.0f;
    nsMatrix _projView;
    nsMatrix _model;
};
