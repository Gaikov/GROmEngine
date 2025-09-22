//
// Created by Roman on 9/9/2024.
//

#include "GLStencilState.h"
#include "Core/ParseFile.h"
#include "Core/ParserUtils.h"
#include "GLUtils.h"

static std::map<std::string, GLenum> s_ops = {
        {"keep",    GL_KEEP},
        {"zero",    GL_ZERO},
        {"replace", GL_REPLACE},
        {"incr",    GL_INCR},
        {"decr",    GL_DECR},
        {"invert",  GL_INVERT}
};

static GLenum GetStencilOp(script_state_t *ss, const char *name) {
    std::string str = ParseStrP(ss, name, "keep");
    return s_ops.count(str) > 0 ? s_ops[str] : GL_KEEP;
}

nsGLStencilState::~nsGLStencilState() {
    Log::Info("...releasing stencil state: %s", _fileName.AsChar());
}

nsGLStencilState *nsGLStencilState::Load(const char *fileName) {
    if (!StrCheck(fileName)) {
        return nullptr;
    }

    auto state = new nsGLStencilState();
    state->_fileName = fileName;
    state->Reload();
    return state;
}

bool nsGLStencilState::Reload() {
    Log::Info("...loading stencil state: %s", _fileName.AsChar());

    nsParseFile pf;
    auto ss = pf.BeginFile(_fileName);

    _enabled = ParseFloat(ss, "enabled") > 0;

    _opFail = GetStencilOp(ss, "op_fail");
    _opZFail = GetStencilOp(ss, "op_zfail");
    _opZPass = GetStencilOp(ss, "op_zpass");

    _func = GLUtils::GetFunc(ParseStrP(ss, "func", "always"));
    _funcRef = (GLint) ParseFloat(ss, "func_ref", 0);
    _funcMask = (GLuint) ParseFloat(ss, "func_mask", 0xff);

    _stencilMask = (GLuint) ParseFloat(ss, "stencil_mask", 0xff);

    float v[4] = {1, 1, 1, 1};
    if (ParseFloat4(ss, "color_mask", v)) {
        _writeRed = v[0] > 0;
        _writeGreen = v[1] > 0;
        _writeBlue = v[2] > 0;
        _writeAlpha = v[3] > 0;
    }

    return true;
}

void nsGLStencilState::Apply(nsGLStencilState *prev) const {
    if (prev->_enabled != _enabled) {
        GLUtils::SetState(GL_STENCIL_TEST, _enabled);
        GL_CHECK_R("GL_STENCIL_TEST",)
    }

    if (_opFail != prev->_opFail || _opZFail != prev->_opZFail || _opZPass != prev->_opZPass) {
        glStencilOp(_opFail, _opZFail, _opZPass);
        GL_CHECK_R("glStencilOp",)
    }

    if (_refChanged || _func != prev->_func || _funcRef != prev->_funcRef || _funcMask != prev->_funcMask) {
        _refChanged = false;
        glStencilFunc(_func, _funcRef, _funcMask);
        GL_CHECK_R("glStencilFunc",)
    }

    if (_stencilMask != prev->_stencilMask) {
        glStencilMask(_stencilMask);
        GL_CHECK_R("glStencilMask",)
    }

    if (_writeRed != prev->_writeRed
        || _writeBlue != prev->_writeBlue
        || _writeGreen != prev->_writeGreen
        || _writeAlpha != prev->_writeAlpha) {
        glColorMask(_writeRed, _writeGreen, _writeBlue, _writeAlpha);
        GL_CHECK_R("glColorMask",)
    }
}

void nsGLStencilState::ForceApply() const {
    GLUtils::SetState(GL_STENCIL_TEST, _enabled);
    GL_CHECK_R("GL_STENCIL_TEST",)

    glStencilOp(_opFail, _opZFail, _opZPass);
    GL_CHECK_R("glStencilOp",)

    _refChanged = false;
    glStencilFunc(_func, _funcRef, _funcMask);
    GL_CHECK_R("glStencilFunc",)

    glStencilMask(_stencilMask);
    GL_CHECK_R("glStencilMask",)

    glColorMask(_writeRed, _writeGreen, _writeBlue, _writeAlpha);
    GL_CHECK_R("glColorMask",)
}

void nsGLStencilState::SetStencilRef(byte value) {
    if (_funcRef != value) {
        _funcRef = value;
        _refChanged = true;
    }
}
