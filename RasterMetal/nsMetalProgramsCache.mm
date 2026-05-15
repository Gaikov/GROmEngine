//
// Created by OpenCode
//

#include "nsMetalProgramsCache.h"
#include "nsLib/log.h"
#include <cstring>

nsMetalProgramsCache::~nsMetalProgramsCache() {
    Release();
}

bool nsMetalProgramsCache::Init() {
    _defaultProgram = GetProgram(DEFAULT_VERTEX_SHADER, DEFAULT_FRAGMENT_SHADER);
    if (!_defaultProgram) {
        Log::Error("Can't load default Metal program!");
        return false;
    }
    return true;
}

void nsMetalProgramsCache::Release() {
    delete _defaultProgram;
    _defaultProgram = nullptr;
    _currentProgram = nullptr;
    for (auto &it : _cache) {
        delete it.second;
    }
    _cache.clear();
}

nsMetalProgram* nsMetalProgramsCache::GetProgram(const char *vertexShaderPath,
                                                  const char *fragmentShaderPath) {
    const auto key = std::string(vertexShaderPath) + "|" + fragmentShaderPath;
    auto it = _cache.find(key);
    if (it != _cache.end()) {
        return it->second;
    }

    auto program = new nsMetalProgram(_device);
    if (!program->Load(vertexShaderPath, fragmentShaderPath)) {
        delete program;
        return nullptr;
    }
    _cache[key] = program;
    return program;
}

void nsMetalProgramsCache::Bind(nsMetalProgram *program, bool force) {
    if (program != _currentProgram || force) {
        _currentProgram = program;
    }
}

void nsMetalProgramsCache::SetTextureBound(bool hasBound) {
    _textureBound = hasBound;
}

void nsMetalProgramsCache::SetTextureMatrix(const nsMatrix &m) {
    _textureMatrix = m;
}

void nsMetalProgramsCache::SetAlphaCutoff(float alphaRef) {
    _alphaCutoff = alphaRef;
}

void nsMetalProgramsCache::SetProjViewMatrix(const float *m) {
    _projView = m;
}

void nsMetalProgramsCache::SetModelMatrix(const float *m) {
    _model = m;
}
