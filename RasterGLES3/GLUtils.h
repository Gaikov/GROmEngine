//
// Created by Roman Gaikov on 2019-07-18.
//

#ifndef SDLTEST_GLUTILS_H
#define SDLTEST_GLUTILS_H

#include "GLCommon.h"

class GLUtils
{
public:
	static bool HasError(const char *funcName);
	static void SetState(GLenum state, bool enabled);
    static GLenum GetFunc(const char *name);
};

#ifdef DEBUG

#define GL_CHECK(funcName) \
if (GLUtils::HasError(funcName)) { \
    return false; \
}

#define GL_CHECK_R(funcName, retValue) \
if (GLUtils::HasError(funcName)) { \
    return retValue; \
}

#else

#define GL_CHECK(funcName)
#define GL_CHECK_R(funcName, retValue)

#endif

// Android drivers may synchronize on glGetError. Define GROM_GL_HOT_CHECKS when
// full per-draw validation is needed during a focused debugging session.
#if defined(DEBUG) && (!defined(ANDROID) || defined(GROM_GL_HOT_CHECKS))
#define GL_CHECK_HOT(funcName) GL_CHECK(funcName)
#define GL_CHECK_HOT_R(funcName, retValue) GL_CHECK_R(funcName, retValue)
#else
#define GL_CHECK_HOT(funcName)
#define GL_CHECK_HOT_R(funcName, retValue)
#endif

#endif //SDLTEST_GLUTILS_H
