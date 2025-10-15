//
// Created by Roman on 9/22/2025.
//

#include "GLProgram.h"

#include "GLUtils.h"
#include "Core/Package.h"

nsGLProgram::nsGLProgram(nsGLSLCache &cache) : _codeCache(cache) {
}

void nsGLProgram::Init(const char *vertexShader, const char *fragmentShader) {
    _vertexShader = vertexShader;
    _fragmentShader = fragmentShader;
}

bool nsGLProgram::Load() {
    assert(!_vertexShader.empty() && !_fragmentShader.empty());

    const auto vs = _codeCache.GetResource(_vertexShader.c_str(), 0);
    if (!vs) {
        return false;
    }

    const auto fs = _codeCache.GetResource(_fragmentShader.c_str(), 0);
    if (!fs) {
        return false;
    }

    _program = glCreateProgram();
    GL_CHECK_R("glCreateProgram", false)

    glAttachShader(_program, vs->GetShader());
    glAttachShader(_program, fs->GetShader());
    glLinkProgram(_program);
    GL_CHECK_R("glLinkProgram", false);

    GLint linkOK = GL_FALSE;
    glGetProgramiv(_program, GL_LINK_STATUS, &linkOK);
    if (linkOK != GL_TRUE) {
        GLint logLen = 0;
        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 1) {
            std::string log(logLen, '\0');
            GLsizei written = 0;
            glGetProgramInfoLog(_program, logLen, &written, log.data());
            Log::Error("Program link failed: %s", log.c_str());
        } else {
            Log::Error("Program link failed with no log.");
        }
        return false;
    }


    if (!GetUniformLocation("uProjView", _projView)) {
        return false;
    }

    if (!GetUniformLocation("uModel", _model)) {
        return false;
    }

    if (!GetUniformLocation("uTexMat", _texMat)) {
        return false;
    }

    if (!GetUniformLocation("uAlphaCutoff", _alphaCutoff)) {
        return false;
    }

    if (!GetUniformLocation("uHasTexture", _hasTexture)) {
        return false;
    }

    glUseProgram(_program);
    GL_CHECK_R("glUseProgram", false);

    nsMatrix m;
    m.Identity();

    SetTextureMatrix(m);
    SetAlphaCutoff(0);

    return true;
}

void nsGLProgram::Unload() {
    if (_program) {
        glDeleteProgram(_program);
        _program = 0;
    }
}

bool nsGLProgram::Bind() {
    if (!_program) {
        if (!Load()) {
            return false;
        }
    }

    glUseProgram(_program);
    GL_CHECK_R("glUseProgram", false);
    return true;
}

bool nsGLProgram::SetProjView(const float *matrix) const {
    glUniformMatrix4fv(_projView, 1, GL_FALSE, matrix);
    GL_CHECK_R("glUniformMatrix4fv - SetProjView", false);
    return true;
}

bool nsGLProgram::SetModel(const float *matrix) const {
    glUniformMatrix4fv(_model, 1, GL_FALSE, matrix);
    GL_CHECK_R("glUniformMatrix4fv - SetModel", false);
    return true;
}

void nsGLProgram::SetTextureMatrix(const float *matrix) const {
    glUniformMatrix4fv(_texMat, 1, GL_FALSE,  matrix);
    GL_CHECK_R("glUniformMatrix4fv - SetTextureMatrix",);
}

void nsGLProgram::SetAlphaCutoff(const float cutoff) const {
    glUniform1f(_alphaCutoff, cutoff);
    GL_CHECK_R("glUniform1f - SetAlphaCutoff",);
}

void nsGLProgram::SetHasTexture(const bool hasTexture) const {
    glUniform1i(_hasTexture, hasTexture ? 1 : 0);
    GL_CHECK_R("glUniform1i - SetHasTexture",);
}

bool nsGLProgram::GetUniformLocation(const char *name, GLint &u) const {
    u = glGetUniformLocation(_program, name);
    if (u == -1) {
        Log::Error("Uniform location '%s'", name);
        return false;
    }
    return true;
}
