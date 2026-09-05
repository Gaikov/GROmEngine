//
// Created by Roman on 9/22/2025.
//

#include "GLProgram.h"

#include "GLUtils.h"
#include "Core/Package.h"
#include "Core/RenderStats.h"
#include <cstring>

nsGLProgram::nsGLProgram(nsGLSLCache &cache) : _codeCache(cache) {
}

void nsGLProgram::Init(const char *vertexShader, const char *fragmentShader) {
    _vertexShader = vertexShader;
    _fragmentShader = fragmentShader;
}

bool nsGLProgram::Load() {
    Unload();

    assert(!_vertexShader.empty() && !_fragmentShader.empty());

    const auto vs = _codeCache.GetResource(_vertexShader.c_str(), 0);
    if (!vs || !vs->GetShader()) {
        return false;
    }

    const auto fs = _codeCache.GetResource(_fragmentShader.c_str(), 0);
    if (!fs || !fs->GetShader()) {
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

    ResolveUserUniforms();

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
    nsRenderStats::AddProgramBind();
    GL_CHECK_HOT_R("glUseProgram", false);

    ApplyUserUniforms();

    return true;
}

void nsGLProgram::SetProjView(const float *matrix) const {
    glUniformMatrix4fv(_projView, 1, GL_FALSE, matrix);
    nsRenderStats::AddUniformUpdate();
    GL_CHECK_HOT_R("glUniformMatrix4fv - SetProjView",);
}

void nsGLProgram::SetModel(const float *matrix) const {
    glUniformMatrix4fv(_model, 1, GL_FALSE, matrix);
    nsRenderStats::AddUniformUpdate();
    GL_CHECK_HOT_R("glUniformMatrix4fv - SetModel",);
}

void nsGLProgram::SetTextureMatrix(const float *matrix) const {
    glUniformMatrix4fv(_texMat, 1, GL_FALSE,  matrix);
    nsRenderStats::AddUniformUpdate();
    GL_CHECK_HOT_R("glUniformMatrix4fv - SetTextureMatrix",);
}

void nsGLProgram::SetAlphaCutoff(const float cutoff) const {
    glUniform1f(_alphaCutoff, cutoff);
    nsRenderStats::AddUniformUpdate();
    GL_CHECK_HOT_R("glUniform1f - SetAlphaCutoff",);
}

void nsGLProgram::SetHasTexture(const bool hasTexture) const {
    glUniform1i(_hasTexture, hasTexture ? 1 : 0);
    nsRenderStats::AddUniformUpdate();
    GL_CHECK_HOT_R("glUniform1i - SetHasTexture",);
}

bool nsGLProgram::GetUniformLocation(const char *name, GLint &u) const {
    u = glGetUniformLocation(_program, name);
    if (u == -1) {
        Log::Error("Uniform location '%s'", name);
        return false;
    }
    return true;
}

nsGLProgram::~nsGLProgram() {
    for (auto &it : _userUniforms) {
        delete static_cast<GLUniform *>(it.second);
    }
    _userUniforms.clear();
}

IShaderUniform *nsGLProgram::GetUniform(const char *name) {
    auto it = _userUniforms.find(name);
    if (it != _userUniforms.end()) {
        return it->second;
    }

    auto uniform = new GLUniform(this, name);
    _userUniforms[name] = uniform;
    return uniform;
}

void nsGLProgram::ResolveUserUniforms() {
    for (auto &it : _userUniforms) {
        static_cast<GLUniform *>(it.second)->ResolveAndApply();
    }
}

void nsGLProgram::ApplyUserUniforms() {
    for (auto &it : _userUniforms) {
        static_cast<GLUniform *>(it.second)->Apply();
    }
}

GLUniform::GLUniform(nsGLProgram *program, const char *name)
    : _program(program), _name(name) {
}

void GLUniform::SetFloat(float value) {
    Set(&value, 1);
}

void GLUniform::SetFloat3(const float value[3]) {
    Set(value, 3);
}

void GLUniform::SetFloat4(const float value[4]) {
    Set(value, 4);
}

void GLUniform::Set(const float *value, int count) {
    memcpy(_value, value, sizeof(float) * count);
    _count = count;

    Apply();
}

void GLUniform::ResolveAndApply() {
    _location = glGetUniformLocation(_program->GetGLProgram(), _name.c_str());
    Apply();
}

void GLUniform::Apply() {
    const GLuint program = _program->GetGLProgram();
    if (!program || !_count) {
        return;
    }

    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    if (static_cast<GLuint>(currentProgram) != program) {
        return;
    }
    if (_location < 0) {
        _location = glGetUniformLocation(program, _name.c_str());
        if (_location < 0) return;
    }

    switch (_count) {
        case 1: glUniform1f(_location, _value[0]); break;
        case 3: glUniform3fv(_location, 1, _value); break;
        case 4: glUniform4fv(_location, 1, _value); break;
        default: break;
    }
}
