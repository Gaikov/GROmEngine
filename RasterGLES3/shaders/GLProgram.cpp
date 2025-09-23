//
// Created by Roman on 9/22/2025.
//

#include "GLProgram.h"

#include "GLUtils.h"
#include "Core/Package.h"

nsGLProgram::nsGLProgram(const char *vertexShader, const char *fragmentShader)
    : _vertexShader(vertexShader), _fragmentShader(fragmentShader) {
}

bool nsGLProgram::Load() {
    _vs = createShader(GL_VERTEX_SHADER, _vertexShader.c_str());
    if (!_vs) {
        return false;
    }

    _fs = createShader(GL_FRAGMENT_SHADER, _fragmentShader.c_str());
    if (!_fs) {
        return false;
    }

    _program = glCreateProgram();
    GL_CHECK_R("glCreateProgram", false)

    glAttachShader(_program, _vs);
    glAttachShader(_program, _fs);
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

    glUseProgram(_program);
    GL_CHECK_R("glUseProgram", false);

    nsMatrix m;
    m.Identity();

    if (!SetTextureMatrix(m)) {
        return false;
    }

    if (!SetAlphaCutoff(0)) {
        return false;
    }

    return true;
}

void nsGLProgram::Unload() {
    if (_program) {
        glDeleteProgram(_program);
        _program = 0;
    }

    if (_vs) {
        glDeleteShader(_vs);
        _vs = 0;
    }

    if (_fs) {
        glDeleteShader(_fs);
        _fs = 0;
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

GLuint nsGLProgram::createShader(const GLenum type, const char *filePath) {
    Log::Info("...loading shader: %s", filePath);
    const auto file = g_pack.LoadFile(filePath);
    if (!file) {
        return 0;
    }

    const auto source = reinterpret_cast<GLchar *>(file->GetData());
    const GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    GL_CHECK_R("glShaderSource", 0);

    glCompileShader(shader);
    g_pack.ReleaseFile(file);

    GL_CHECK_R("glCompileShader", 0);
    return shader;
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

bool nsGLProgram::SetTextureMatrix(const float *matrix) const {
    glUniformMatrix4fv(_texMat, 1, GL_FALSE,  matrix);
    GL_CHECK_R("glUniformMatrix4fv - SetTextureMatrix", false);
    return true;
}

bool nsGLProgram::SetAlphaCutoff(const float cutoff) const {
    glUniform1f(_alphaCutoff, cutoff);
    GL_CHECK_R("glUniform1f - SetAlphaCutoff", false);
    return true;
}

bool nsGLProgram::GetUniformLocation(const char *name, GLint &u) const {
    u = glGetUniformLocation(_program, name);
    if (u == -1) {
        Log::Error("Uniform location '%s'", name);
        return false;
    }
    return true;
}
