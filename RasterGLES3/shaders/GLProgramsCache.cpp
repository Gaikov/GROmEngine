//
// Created by Roman on 10/14/2025.
//

#include "GLProgramsCache.h"

nsGLProgramsCache::nsGLProgramsCache() {
    _textureMatrix.Identity();
}

nsGLProgram * nsGLProgramsCache::GetProgram(const char *vertexShaderPath, const char *fragmentShaderPath) {
    std::string path = vertexShaderPath;
    path += "+";
    path += fragmentShaderPath;
    if (_cache.contains(path)) {
        return _cache[path];
    }

    Log::Info("...creating GLSL program: %s", path.c_str());
    const auto program = new nsGLProgram(_codeCache);
    program->Init(vertexShaderPath, fragmentShaderPath);
    return program;
}

void nsGLProgramsCache::Invalidate() const {
    _codeCache.Invalidate();
    for (const auto &it : _cache) {
        it.second->Unload();
    }
}

void nsGLProgramsCache::Bind(nsGLProgram *program, bool force) {
    if (!program) {
        program = _defaultProgram;
    }

    if (force || _currentProgram != program) {
        _currentProgram = program;
        _currentProgram->Bind();
        _currentProgram->SetHasTexture(_textureBound);
        _currentProgram->SetTextureMatrix(_textureMatrix);
        _currentProgram->SetAlphaCutoff(_alphaCutoff);
        _currentProgram->SetProjView(_projView);
        _currentProgram->SetModel(_model);
    }
}

void nsGLProgramsCache::SetTextureBound(const bool hasBound) {
    _textureBound = hasBound;
    _currentProgram->SetHasTexture(_textureBound);
}

void nsGLProgramsCache::SetTextureMatrix(const nsMatrix &m) {
    _textureMatrix = m;
    _currentProgram->SetTextureMatrix(_textureMatrix);
}

void nsGLProgramsCache::SetAlphaCutoff(const float alphaRef) {
    _alphaCutoff = alphaRef;
    _currentProgram->SetAlphaCutoff(_alphaCutoff);
}

void nsGLProgramsCache::SetProjViewMatrix(const float *m) {
    _projView = m;
    _currentProgram->SetProjView(_projView);
}

void nsGLProgramsCache::SetModelMatrix(const float *m) {
    _model = m;
    _currentProgram->SetModel(_model);
}

void nsGLProgramsCache::Release() {
    Log::Info("...releasing GLSL programs");
    Invalidate();
    for (const auto &it : _cache) {
        delete it.second;
    }
    _cache.clear();
    _codeCache.ReleaseAll();
}

bool nsGLProgramsCache::Init() {
    _defaultProgram = GetProgram(DEFAULT_VERTEX_SHADER, DEFAULT_FRAGMENT_SHADER);
    if (!_defaultProgram->Load()) {
        Sys_FatalError("Failed to load default program");
        return false;
    }

    Bind(nullptr, true);
    return true;
}

