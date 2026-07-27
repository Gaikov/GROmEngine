//
// Created by Roman on 4/12/2024.
//

#include "GLNativeContext.h"
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include "nsLib/log.h"

GLNativeContext::GLNativeContext(EGLNativeWindowType win) :
        _win(win),
        _display(EGL_NO_DISPLAY),
        _surface(EGL_NO_SURFACE),
        context_(EGL_NO_CONTEXT),
        width_(0),
        height_(0) {

}

void GLNativeContext::BeginRender() {
    EGLint width = GetSurfaceWidth();
    EGLint height = GetSurfaceHeight();

    if (width != width_ || height != height_) {
        width_ = width;
        height_ = height;
        glViewport(0, 0, width, height);
    }
}

void GLNativeContext::SwapBuffers() {
    if (_display != EGL_NO_DISPLAY && _surface != EGL_NO_SURFACE) {
        eglSwapBuffers(_display, _surface);
    }
}

bool GLNativeContext::Init() {
    constexpr EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
    };

    _display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (_display == EGL_NO_DISPLAY) {
        Log::Error("Failed to get EGL display: 0x%x", eglGetError());
        return false;
    }

    if (eglInitialize(_display, nullptr, nullptr) != EGL_TRUE) {
        Log::Error("Failed to initialize EGL: 0x%x", eglGetError());
        Release();
        return false;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE) {
        Log::Error("Failed to bind OpenGL ES API: 0x%x", eglGetError());
        Release();
        return false;
    }

    EGLint numConfigs = 0;
    if (eglChooseConfig(_display, attribs, nullptr, 0, &numConfigs) != EGL_TRUE ||
        numConfigs <= 0) {
        Log::Error("No EGL config supports OpenGL ES 3: 0x%x", eglGetError());
        Release();
        return false;
    }

    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
    if (eglChooseConfig(_display, attribs, supportedConfigs.get(), numConfigs, &numConfigs) != EGL_TRUE) {
        Log::Error("Failed to query EGL configs: 0x%x", eglGetError());
        Release();
        return false;
    }

    auto configIt = std::find_if(
            supportedConfigs.get(),
            supportedConfigs.get() + numConfigs,
            [this](const EGLConfig &config) {
                EGLint red = 0;
                EGLint green = 0;
                EGLint blue = 0;
                EGLint depth = 0;
                EGLint stencil = 0;
                eglGetConfigAttrib(_display, config, EGL_RED_SIZE, &red);
                eglGetConfigAttrib(_display, config, EGL_GREEN_SIZE, &green);
                eglGetConfigAttrib(_display, config, EGL_BLUE_SIZE, &blue);
                eglGetConfigAttrib(_display, config, EGL_DEPTH_SIZE, &depth);
                eglGetConfigAttrib(_display, config, EGL_STENCIL_SIZE, &stencil);
                return red == 8 && green == 8 && blue == 8 &&
                       depth >= 24 && stencil >= 8;
            });

    if (configIt == supportedConfigs.get() + numConfigs) {
        Log::Error("No compatible RGBA8 depth/stencil EGL config found");
        Release();
        return false;
    }
    const EGLConfig config = *configIt;

    _surface = eglCreateWindowSurface(_display, config, _win, nullptr);
    if (_surface == EGL_NO_SURFACE) {
        Log::Error("Failed to create EGL window surface: 0x%x", eglGetError());
        Release();
        return false;
    }

    constexpr EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
    };
    context_ = eglCreateContext(_display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context_ == EGL_NO_CONTEXT) {
        Log::Error("Failed to create OpenGL ES 3 context: 0x%x", eglGetError());
        Release();
        return false;
    }

    if (eglMakeCurrent(_display, _surface, _surface, context_) != EGL_TRUE) {
        Log::Error("Failed to activate OpenGL ES 3 context: 0x%x", eglGetError());
        Release();
        return false;
    }

    width_ = -1;
    height_ = -1;

    Log::Info("OpenGL ES 3 EGL context initialized");
    return true;
}

EGLint GLNativeContext::GetSurfaceWidth() {
    assert(_display && _surface);

    EGLint width;
    eglQuerySurface(_display, _surface, EGL_WIDTH, &width);
    return width;
}

EGLint GLNativeContext::GetSurfaceHeight() {
    assert(_display && _surface);

    EGLint height;
    eglQuerySurface(_display, _surface, EGL_HEIGHT, &height);
    return height;
}

void GLNativeContext::Release() {
    if (_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context_ != EGL_NO_CONTEXT) {
            eglDestroyContext(_display, context_);
            context_ = EGL_NO_CONTEXT;
        }
        if (_surface != EGL_NO_SURFACE) {
            eglDestroySurface(_display, _surface);
            _surface = EGL_NO_SURFACE;
        }
        eglTerminate(_display);
        _display = EGL_NO_DISPLAY;
    }
    width_ = 0;
    height_ = 0;
}
