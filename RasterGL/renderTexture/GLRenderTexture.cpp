//
// Created by Roman on 9/18/2025.
//

#include "GLRenderTexture.h"
#include "GLCommon.h"
#include "GLUtils.h"

static PFNGLGENFRAMEBUFFERSOESPROC         glGenFramebuffersOES         = nullptr;
static PFNGLDELETEFRAMEBUFFERSOESPROC      glDeleteFramebuffersOES      = nullptr;
static PFNGLBINDFRAMEBUFFEROESPROC         glBindFramebufferOES         = nullptr;
static PFNGLFRAMEBUFFERTEXTURE2DOESPROC    glFramebufferTexture2DOES    = nullptr;
static PFNGLFRAMEBUFFERRENDERBUFFEROESPROC glFramebufferRenderbufferOES = nullptr;
static PFNGLCHECKFRAMEBUFFERSTATUSOESPROC  glCheckFramebufferStatusOES  = nullptr;

static PFNGLGENRENDERBUFFERSOESPROC        glGenRenderbuffersOES        = nullptr;
static PFNGLDELETERENDERBUFFERSOESPROC     glDeleteRenderbuffersOES     = nullptr;
static PFNGLBINDRENDERBUFFEROESPROC        glBindRenderbufferOES        = nullptr;
static PFNGLRENDERBUFFERSTORAGEOESPROC     glRenderbufferStorageOES     = nullptr;

#define FRAME_BUFFER_EXT_NAME "GL_OES_framebuffer_object"
#define FRAME_BUFFER_EXT_NAME_DESK "GL_ARB_framebuffer_object"

static Platform::ProcAddr GetProcAddr(const char *name) {
    const auto p = App_GetPlatform();
    std::string procName = name;
    auto addr = p->GetProcAddr((procName + "OES").c_str());
    if (!addr) {
        procName = name;
        addr = p->GetProcAddr(procName.c_str());
    }

    Log::Info("%s: %s", procName.c_str(), addr ? "OK" : "None");
    return addr;
}

bool nsGLRenderTexture::InitGLExtensionsFBO() {
    Log::Info("Init GL extension: %s", FRAME_BUFFER_EXT_NAME);
    const std::string_view s = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));
    bool hasExt = s.find(FRAME_BUFFER_EXT_NAME) != std::string_view::npos || s.find(FRAME_BUFFER_EXT_NAME_DESK) != std::string_view::npos;
    if (!hasExt) {
        Log::Error("Can't find extension: %s", FRAME_BUFFER_EXT_NAME);
        return false;
    }

    glGenFramebuffersOES         = (PFNGLGENFRAMEBUFFERSOESPROC)        GetProcAddr("glGenFramebuffers");
    glDeleteFramebuffersOES      = (PFNGLDELETEFRAMEBUFFERSOESPROC)     GetProcAddr("glDeleteFramebuffers");
    glBindFramebufferOES         = (PFNGLBINDFRAMEBUFFEROESPROC)        GetProcAddr("glBindFramebuffer");
    glFramebufferTexture2DOES    = (PFNGLFRAMEBUFFERTEXTURE2DOESPROC)   GetProcAddr("glFramebufferTexture2D");
    glFramebufferRenderbufferOES = (PFNGLFRAMEBUFFERRENDERBUFFEROESPROC)GetProcAddr("glFramebufferRenderbuffer");
    glCheckFramebufferStatusOES  = (PFNGLCHECKFRAMEBUFFERSTATUSOESPROC) GetProcAddr("glCheckFramebufferStatus");

    glGenRenderbuffersOES        = (PFNGLGENRENDERBUFFERSOESPROC)       GetProcAddr("glGenRenderbuffers");
    glDeleteRenderbuffersOES     = (PFNGLDELETERENDERBUFFERSOESPROC)    GetProcAddr("glDeleteRenderbuffers");
    glBindRenderbufferOES        = (PFNGLBINDRENDERBUFFEROESPROC)       GetProcAddr("glBindRenderbuffer");
    glRenderbufferStorageOES     = (PFNGLRENDERBUFFERSTORAGEOESPROC)    GetProcAddr("glRenderbufferStorage");

    hasExt = glGenFramebuffersOES && glBindFramebufferOES && glGenRenderbuffersOES;
    if (!hasExt) {
        Log::Error("Can't init extension functions: %s", FRAME_BUFFER_EXT_NAME);
        hasExt = false;
    }
    return hasExt;
}

void nsGLRenderTexture::Unbind() {
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
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
            glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
            return false;
        }
    }

    glBindFramebufferOES(GL_FRAMEBUFFER_OES, _fbo);
    GL_CHECK_R("glBindFramebufferOES", false);
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

    glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);

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
