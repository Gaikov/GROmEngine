//
// Created by Roman on 10/14/2025.
//

#include "GLProgramsCache.h"

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
    }
}

void nsGLProgramsCache::SetTextureBound(const bool hasBound) const {
    _currentProgram->SetHasTexture(hasBound);
}

void nsGLProgramsCache::SetTextureMatrix(const nsMatrix &m) const {
    _currentProgram->SetTextureMatrix(m);
}

void nsGLProgramsCache::SetAlphaCutoff(const float alphaRef) const {
    _currentProgram->SetAlphaCutoff(alphaRef);
}

void nsGLProgramsCache::SetProjView(const float *m) const {
    _currentProgram->SetProjView(m);
}

void nsGLProgramsCache::SetModel(const float *m) const {
    _currentProgram->SetModel(m);
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

