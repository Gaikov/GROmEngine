//
// Created by Roman on 10/8/2025.
//

#include "GLSLCode.h"

#include "GLUtils.h"
#include "Core/Package.h"
#include "nsLib/utils/AutoDestroy.h"

static GLenum ShaderTypeFromExtension(const char *path) {
    if (!StrCheck(path)) {
        Log::Error("Invalid shader path: %s", path);
        return 0;
    }

    const nsFilePath filePath(path);
    const nsString ext = filePath.GetExtension();

    if (ext == "vert" || ext == "vs" || ext == "vsh")
        return GL_VERTEX_SHADER;

    if (ext == "frag" || ext == "fs" || ext == "fsh")
        return GL_FRAGMENT_SHADER;

    return 0;
}

nsGLSLCode::nsGLSLCode(const char *filePath) : resourceName(filePath) {
}

nsGLSLCode::~nsGLSLCode() {
    Unload();
}

GLuint nsGLSLCode::GetShader() {
    if (!_shader) {
        Load();
    }
    return _shader;
}

void nsGLSLCode::Invalidate() {
    Unload();
}

void nsGLSLCode::Load() {
    const GLenum type = ShaderTypeFromExtension(resourceName);
    if (type == 0) {
        Log::Error("Unknown GLSL extension: %s", resourceName.AsChar());
        return;
    }

    Log::Info("...loading GLSL: %s", resourceName);
    const auto file = g_pack.LoadFile(resourceName);
    if (!file) {
        Log::Error("Failed to load shader file: %s", resourceName);
        return;
    }

    nsAutoDestroy ad;
    ad.AddDestroyCallback([file] {
        g_pack.ReleaseFile(file);
    });

    const auto src = reinterpret_cast<const GLchar *>(file->GetData());
    _shader = glCreateShader(type);
    GL_CHECK_R("glCreateShader",);

    glShaderSource(_shader, 1, &src, nullptr);
    GL_CHECK_R("glShaderSource",);

    glCompileShader(_shader);
    GL_CHECK_R("glCompileShader",);

    // Validate compilation status and print log if any
    GLint compiled = GL_FALSE;
    glGetShaderiv(_shader, GL_COMPILE_STATUS, &compiled);

    GLint logLen = 0;
    glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &logLen);
    if (logLen > 1) {
        std::string log(static_cast<size_t>(logLen), '\0');
        GLsizei written = 0;
        glGetShaderInfoLog(_shader, logLen, &written, log.data());
        if (!log.empty())
            Log::Info("Shader log [%s]: %s", resourceName, log.c_str());
    }

    if (compiled != GL_TRUE) {
        Log::Error("Shader compile failed: %s", resourceName);
        glDeleteShader(_shader);
        _shader = 0;
    }
}

void nsGLSLCode::Unload() {
    if (_shader) {
        Log::Info("...unloading GLSL: %s", resourceName);
        glDeleteShader(_shader);
        _shader = 0;
    }
}
