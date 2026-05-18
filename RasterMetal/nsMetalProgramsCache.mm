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
    _textureMatrix.Identity();
    _projView.Identity();
    _model.Identity();

    _defaultProgram = GetProgram(DEFAULT_VERTEX_SHADER, DEFAULT_FRAGMENT_SHADER);
    if (!_defaultProgram) {
        Log::Error("Can't load default Metal program!");
        return false;
    }
    return true;
}

void nsMetalProgramsCache::Release() {
    _currentProgram = nullptr;
    _defaultProgram = nullptr;

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
        if (_currentProgram) {
            _currentProgram->SetProjView(_projView);
            _currentProgram->SetModel(_model);
            _currentProgram->SetHasTexture(_textureBound);
            _currentProgram->SetTextureMatrix(_textureMatrix);
            _currentProgram->SetAlphaCutoff(_alphaCutoff);
        }
    }
}

void nsMetalProgramsCache::SetTextureBound(bool hasBound) {
    _textureBound = hasBound;
    if (_currentProgram) {
        _currentProgram->SetHasTexture(hasBound);
    }
}

void nsMetalProgramsCache::SetTextureMatrix(const nsMatrix &m) {
    _textureMatrix = m;
    if (_currentProgram) {
        _currentProgram->SetTextureMatrix(m);
    }
}

void nsMetalProgramsCache::SetAlphaCutoff(float alphaRef) {
    _alphaCutoff = alphaRef;
    if (_currentProgram) {
        _currentProgram->SetAlphaCutoff(alphaRef);
    }
}

void nsMetalProgramsCache::SetProjViewMatrix(const float *m) {
    _projView = m;
    if (_currentProgram) {
        _currentProgram->SetProjView(m);
    }
}

void nsMetalProgramsCache::SetModelMatrix(const float *m) {
    _model = m;
    if (_currentProgram) {
        _currentProgram->SetModel(m);
    }
}
