// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLBaseTexture.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/RenDevice.h"
#include "GLCommon.h"

// Resource uploads must not overwrite the texture binding owned by the draw cache.
class nsGLTextureBindingScope final {
public:
    nsGLTextureBindingScope() { glGetIntegerv(GL_TEXTURE_BINDING_2D, &_previous); }
    ~nsGLTextureBindingScope() { glBindTexture(GL_TEXTURE_2D, _previous); }
    nsGLTextureBindingScope(const nsGLTextureBindingScope &) = delete;
    nsGLTextureBindingScope &operator=(const nsGLTextureBindingScope &) = delete;

private:
    GLint _previous = 0;
};

class nsGLBaseTexture : public ITexture {
public:
    virtual bool Bind() = 0;
    virtual bool IsMipmapped() const = 0;
    unsigned GetBindingVersion() const { return _bindingVersion; }

protected:
    unsigned _bindingVersion = 0;
};
