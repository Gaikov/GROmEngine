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

    void SwapBuffers();

    EGLint GetSurfaceWidth();
    EGLint GetSurfaceHeight();

private:
    EGLNativeWindowType _win;
    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext context_;
    EGLint width_;
    EGLint height_;
};


#endif //_GROM_EGLCONTEXT_H
