// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RenState.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "GLCommon.h"
#include "GLProgramsCache.h"
#include "Core/Parser.h"

class GLShader : public IRenState
{
public:
    static constexpr auto SamplerClampU = 1u << 0;
    static constexpr auto SamplerClampV = 1u << 1;
    static constexpr auto SamplerNearest = 1u << 2;
    static constexpr auto SamplerStateCount = (SamplerClampU | SamplerClampV | SamplerNearest) + 1u;

    nsString    fileName;
    GLShader(nsGLProgramsCache &programs);
    const char* GetPath() const { return fileName; }
    bool        Reload();

    void        ForceApply();
    void        Apply( GLShader *prev );
    unsigned int GetSamplerIndex() const;

    bool        IsAlphaTest() override { return _alphaTest != 0; }
    float        GetAlphaCutoff() const { return _alphaRef; }

    IShaderUniform *GetUniform(const char *name) override;

private:
    bool        _zEnable;
    GLboolean    _zWrite;
    float _depthBias = 0;
    float _slopeScaleDepthBias = 0;

    bool        _alphaTest;
    GLclampf    _alphaRef;

    bool        _alphaBlend;
    GLenum        _srcBlend;
    GLenum        _dstBlend;

    GLenum        _srcAlphaBlend = GL_ONE;
    GLenum        _dstAlphaBlend = GL_ONE;

    bool        _cullMode;

    GLint        _texCoordU;
    GLint        _texCoordV;
    bool        _nearestFilter = false;

    nsGLProgram    *_program = nullptr;
    nsGLProgramsCache &_programs;

private:
    bool        Parse( script_state_t *ss );

    bool        IsAlphaBlend() override { return _alphaBlend != 0; }
};
