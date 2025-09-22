// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RenState.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_RenState_H_
#define	_RenState_H_

#include "GLCommon.h"
#include "Core/Parser.h"

class GLShader : public IRenState
{
public:
	nsString	m_fileName;

public:
	GLShader();
	bool		Reload();

	void		ForceApply();
	void		Apply( GLShader *prev );
	void        ApplyTextureParams() const;

private:
	bool		m_zEnable;
	GLboolean	m_zWrite;

	bool		m_alphaTest;
	GLclampf	m_alphaRef;

	bool		m_alphaBlend;
	GLenum		m_srcBlend;
	GLenum		m_dstBlend;

	bool		m_cullMode;

	GLint		m_texCoordU;
	GLint		m_texCoordV;

private:
	bool		Parse( script_state_t *ss );
	bool        IsAlphaTest() override { return m_alphaTest != 0; }
	bool	    IsAlphaBlend() override { return m_alphaBlend != 0; }
};

#endif //_RenState_H_