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

#endif //SDLTEST_GLUTILS_H
