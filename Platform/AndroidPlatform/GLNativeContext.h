//
// Created by Roman on 4/12/2024.
//

#ifndef _GROM_EGLCONTEXT_H
#define _GROM_EGLCONTEXT_H

#include <EGL/egl.h>

class GLNativeContext {
public:
    GLNativeContext(EGLNativeWindowType win);

    bool Init();
    void Release();

    void BeginRender();

    bool SwapBuffers();
    bool NeedsRecreation() const { return _needsRecreation; }

    EGLint GetSurfaceWidth() const;
    EGLint GetSurfaceHeight() const;

private:
    EGLNativeWindowType _win;
    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext context_;
    EGLint width_;
    EGLint height_;
    int _appliedSwapInterval = -1;
    mutable bool _needsRecreation = false;
};


#endif //_GROM_EGLCONTEXT_H
