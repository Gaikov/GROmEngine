//
// Created by Roman on 10/14/2025.
//

#include "GLProgramsCache.h"

#include <cstring>

namespace {
bool MatricesEqual(const nsMatrix &left, const float *right) {
    return std::memcmp(static_cast<const float *>(left), right, sizeof(nsMatrix)) == 0;
}
}

nsGLProgramsCache::nsGLProgramsCache() {
    _textureMatrix.Identity();
    _projView.Identity();
    _model.Identity();
}

std::string nsGLProgramsCache::ResolveShaderPath(const std::string &path, bool isVertex) {
    constexpr const char *platformToken = "{platform}";
    const auto pos = path.find(platformToken);
    if (pos == std::string::npos) {
        return path;
    }

    std::string result = path;
    result.replace(pos, strlen(platformToken), "gles3");
    result += isVertex ? ".vert" : ".frag";
    return result;
}

nsGLProgram * nsGLProgramsCache::GetProgram(const char *vertexShaderPath, const char *fragmentShaderPath) {
    if (!vertexShaderPath) {
        vertexShaderPath = DEFAULT_VERTEX_SHADER;
    }
    if (!fragmentShaderPath) {
        fragmentShaderPath = DEFAULT_FRAGMENT_SHADER;
    }

    std::string path = vertexShaderPath;
    path += "+";
    path += fragmentShaderPath;
    if (_cache.contains(path)) {
        return _cache[path];
    }

    Log::Info("...creating GLSL program: %s", path.c_str());
    const auto program = new nsGLProgram(_codeCache);
    program->Init(vertexShaderPath, fragmentShaderPath);
    _cache[path] = program;
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
    if (_textureBound == hasBound) return;
    _textureBound = hasBound;
    if (_currentProgram) _currentProgram->SetHasTexture(_textureBound);
}

void nsGLProgramsCache::SetTextureMatrix(const nsMatrix &m) {
    if (MatricesEqual(_textureMatrix, m)) return;
    _textureMatrix = m;
    if (_currentProgram) _currentProgram->SetTextureMatrix(_textureMatrix);
}

void nsGLProgramsCache::SetAlphaCutoff(const float alphaRef) {
    if (_alphaCutoff == alphaRef) return;
    _alphaCutoff = alphaRef;
    if (_currentProgram) _currentProgram->SetAlphaCutoff(_alphaCutoff);
}

void nsGLProgramsCache::SetProjViewMatrix(const float *m) {
    if (MatricesEqual(_projView, m)) return;
    _projView = m;
    if (_currentProgram) _currentProgram->SetProjView(_projView);
}

void nsGLProgramsCache::SetModelMatrix(const float *m) {
    if (MatricesEqual(_model, m)) return;
    _model = m;
    if (_currentProgram) _currentProgram->SetModel(_model);
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
