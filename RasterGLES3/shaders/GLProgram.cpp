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
    GL_CHECK_R("glLinkProgram", false)

    _projView = glGetUniformLocation(_program, "uProjView");
    GL_CHECK_R("glGetUniformLocation(uProjView)", false);

    _model = glGetUniformLocation(_program, "uModel");
    GL_CHECK_R("glGetUniformLocation(uModel)", false);

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
    glCompileShader(shader);

    g_pack.ReleaseFile(file);

    GL_CHECK_R("glCompileShader", 0);
    return shader;
}

void nsGLProgram::SetProjView(const float *projView) const {
    glUniformMatrix4fv(_projView, 1, GL_FALSE, projView);
    GL_CHECK_R("glUniformMatrix4fv - SetProjView",);
}

void nsGLProgram::SetModel(const float *model) const {
    glUniformMatrix4fv(_model, 1, GL_FALSE, model);
    GL_CHECK_R("glUniformMatrix4fv - SetModel",);
}
