//
// Created by Roman on 4/12/2024.
//

#include <GLES/gl.h>
#include "GLNativeContext.h"
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
    // Present the rendered image. This is an implicit glFlush.
    auto swapResult = eglSwapBuffers(_display, _surface);
    assert(swapResult == EGL_TRUE);
}

bool GLNativeContext::Init() {
    // Choose your render attributes
    constexpr EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
    };

    // The default display is probably what you want on Android
    auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, nullptr, nullptr);

    // figure out how many configs there are
    EGLint numConfigs;
    eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);

    // get the list of configurations
    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
    eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);

    // Find a config we like.
    // Could likely just grab the first if we don't care about anything else in the config.
    // Otherwise hook in your own heuristic
    auto config = *std::find_if(
            supportedConfigs.get(),
            supportedConfigs.get() + numConfigs,
            [&display](const EGLConfig &config) {
                EGLint red, green, blue, depth;
                if (eglGetConfigAttrib(display, config, EGL_RED_SIZE, &red)
                    && eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &green)
                    && eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &blue)
                    && eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE, &depth)) {

                    Log::Info("Found config with %i:%i:%i, %i", red, green, blue, depth);
                    return red == 8 && green == 8 && blue == 8 && depth == 24;
                }
                return false;
            });

    Log::Info("Found %i configs", numConfigs);

    // create the proper window surface
    EGLint format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    EGLSurface surface = eglCreateWindowSurface(display, config, _win, nullptr);

    // Create a GLES 1 context
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 1, EGL_NONE};
    auto context = eglCreateContext(display, config, nullptr, contextAttribs);

    // get some window metrics
    auto madeCurrent = eglMakeCurrent(display, surface, surface, context);
    assert(madeCurrent);

    _display = display;
    _surface = surface;
    context_ = context;

    // make width and height invalid so it gets updated the first frame in @a updateRenderArea()
    width_ = -1;
    height_ = -1;

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
        //Log::Info("...releasing EGL context");
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
}
