//
// Created by Roman on 9/18/2025.
//

#include "GLRenderTexture.h"
#include "GLCommon.h"
#include "GLUtils.h"

nsGLRenderTexture::nsGLRenderTexture(const int width, const int height, const texfmt_t fmt)
    : _width(width), _height(height), _fmt(fmt) {
    UploadToGPU();
}

nsGLRenderTexture::~nsGLRenderTexture() {
    UnloadFromGPU();
}

bool nsGLRenderTexture::BindTarget() {
    if (!_fbo || !UploadToGPU()) {
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
        return false;
    }

    glBindFramebufferOES(GL_FRAMEBUFFER_OES, _fbo);
    GL_CHECK_R("glBindFramebufferOES", false);
    glViewport(0, 0, _width, _height);

    return true;
}

bool nsGLRenderTexture::BindTexture() {
    if (!_fbo || !UploadToGPU()) {
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, _texture);
    GL_CHECK_R("glBindTexture", false);

    return true;
}

bool nsGLRenderTexture::UploadToGPU() {
    UnloadFromGPU();

    // texture
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);

    const auto fmt = _fmt == TF_RGBA ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, _width, _height, 0, fmt, GL_UNSIGNED_BYTE, nullptr);
    GL_CHECK_R("glTexImage2D", false);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // depth with stencil
    glGenRenderbuffersOES(1, &_zstencil);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, _zstencil);
    glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH24_STENCIL8_OES, _width, _height);
    GL_CHECK_R("glRenderbufferStorageOES - GL_DEPTH24_STENCIL8_OES", false);

    // frame buffer object
    glGenFramebuffersOES(1, &_fbo);
    GL_CHECK_R("glGenFramebuffersOES", false);
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, _fbo);

    glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, _texture, 0);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, _zstencil);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_STENCIL_ATTACHMENT_OES, GL_RENDERBUFFER_OES, _zstencil);

    // check status
    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
        Log::Error("Can't create Frame Buffer: %i %i", _width, _height);
        return false;
    }

    return true;
}

void nsGLRenderTexture::UnloadFromGPU() {
    if (_fbo) {
        Log::Info("Release render texture: %ix%i", _width, _height);

        glDeleteFramebuffersOES(1, &_fbo);
        _fbo = 0;
    }
    if (_zstencil) {
        glDeleteRenderbuffersOES(1, &_zstencil);
        _zstencil = 0;
    }
    if (_texture) {
        glDeleteTextures(1, &_texture);
        _texture = 0;
    }
}

void nsGLRenderTexture::GetSize(int &width, int &height) {
    width = _width;
    height = _height;
}
