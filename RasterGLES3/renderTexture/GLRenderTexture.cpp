//
// Created by Roman on 9/18/2025.
//

#include "GLRenderTexture.h"
#include "GLCommon.h"
#include "GLUtils.h"

#define FRAME_BUFFER_EXT_NAME "GL_framebuffer_object"
#define FRAME_BUFFER_EXT_NAME_DESK "GL_ARB_framebuffer_object"

bool nsGLRenderTexture::InitGLExtensionsFBO() {
    Log::Info("Init GL extension: %s", FRAME_BUFFER_EXT_NAME);
    const std::string_view s = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));
    const bool hasExt = s.find(FRAME_BUFFER_EXT_NAME) != std::string_view::npos
        || s.find(FRAME_BUFFER_EXT_NAME_DESK) != std::string_view::npos;
    if (!hasExt) {
        Log::Error("Can't find extension: %s", FRAME_BUFFER_EXT_NAME);
        return false;
    }

    GLint maxRbSize;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRbSize);
    Log::Info("Max render buffer size: %i", maxRbSize);

    return hasExt;
}

void nsGLRenderTexture::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int w, h;
    App_GetPlatform()->GetClientSize(w, h);
    glViewport(0, 0, w, h);
}

nsGLRenderTexture::nsGLRenderTexture(const int width, const int height, const texfmt_t fmt)
    : _width(width), _height(height), _fmt(fmt) {
    Log::Info("...creating render texture: %ix%i", _width, _height);
    UploadToGPU();
}

nsGLRenderTexture::~nsGLRenderTexture() {
    UnloadFromGPU();
}

bool nsGLRenderTexture::BindTarget() {
    if (!_fbo) {
        if (!UploadToGPU()) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return false;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    GL_CHECK_R("glBindFramebuffer", false);
    glViewport(0, 0, _width, _height);

    return true;
}

bool nsGLRenderTexture::Bind() {
    if (!_fbo) {
        if (!UploadToGPU()) {
            glBindTexture(GL_TEXTURE_2D, 0);
            return false;
        }
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
    glGenRenderbuffers(1, &_zstencil);
    glBindRenderbuffer(GL_RENDERBUFFER, _zstencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
    GL_CHECK_R("glRenderbufferStorage - GL_DEPTH24_STENCIL8", false);

    // frame buffer object
    glGenFramebuffers(1, &_fbo);
    GL_CHECK_R("glGenFramebuffers", false);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _zstencil);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _zstencil);

    // check status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Log::Error("Can't create Frame Buffer: %i %i", _width, _height);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void nsGLRenderTexture::UnloadFromGPU() {
    if (_fbo) {
        Log::Info("Release render texture: %ix%i", _width, _height);

        glDeleteFramebuffers(1, &_fbo);
        _fbo = 0;
    }
    if (_zstencil) {
        glDeleteRenderbuffers(1, &_zstencil);
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
