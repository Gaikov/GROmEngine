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

void nsGLProgramsCache::Release() {
    Log::Info("...releasing GLSL programs");
    Invalidate();
    for (const auto &it : _cache) {
        delete it.second;
    }
    _cache.clear();
    _codeCache.ReleaseAll();
}

