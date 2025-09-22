//
// Created by Roman Gaikov on 2019-07-18.
//

#include "GLUtils.h"
#include "GLCommon.h"

struct glError_t
{
	GLenum code;
	const char *message;
};

static glError_t errors[] = {
		{
				GL_INVALID_ENUM,
				"An unacceptable value is specified for an enumerated argument."
		},

		{
				GL_INVALID_VALUE,
				"A numeric argument is out of range."
		},

		{
				GL_INVALID_OPERATION,
				"The specified operation is not allowed in the current state."
		},
        /*{
                GL_STACK_OVERFLOW,
                "GL_STACK_OVERFLOW"
        },
        {
                GL_STACK_UNDERFLOW,
                "GL_STACK_UNDERFLOW"
        },*/
		{
				GL_INVALID_FRAMEBUFFER_OPERATION,
				"The command is trying to render to or read from the framebuffer while the currently bound framebuffer is not framebuffer complete (i.e. the return value from glCheckFramebufferStatus is not GL_FRAMEBUFFER_COMPLETE)."
		},
		{
				GL_OUT_OF_MEMORY,
				"There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded."
		}
};

bool GLUtils::HasError(const char *funcName)
{
	auto code = glGetError();
	if (!code)
	{
		return false;
	}

	for (auto error : errors)
	{
		if (error.code == code)
		{
			LogPrintf(PRN_ALL, "GL ERROR: %s: %s\n", funcName, error.message);
			return true;
		}
	}

	LogPrintf(PRN_ALL, "GL ERROR: unknown error\n");
	return true;
}

void GLUtils::SetState(GLenum state, bool enabled)
{
	if (enabled)
	{
		glEnable(state);
	}
	else
	{
		glDisable(state);
	}

}

static std::map<std::string_view, GLenum> s_func = {
        {"never", GL_NEVER},
        {"less", GL_LESS},
        {"equal", GL_EQUAL},
        {"lequal", GL_LEQUAL},
        {"greater", GL_GREATER},
        {"notequal", GL_NOTEQUAL},
        {"gequal", GL_GEQUAL},
        {"always", GL_ALWAYS}
};

GLenum GLUtils::GetFunc(const char *name) {
    return s_func.contains(name) > 0 ? s_func[name] : GL_ALWAYS;
}
