// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RenState.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "GLShader.h"
#include "Core/ParseFile.h"
#include "nsLib/log.h"
#include "GLUtils.h"

struct renValueMap_t
{
	const char	*name;
	GLenum		value;
};

#define RENMAP_COUNT( array ) sizeof(array) / sizeof(renValueMap_t)

//---------------------------------------------------------
// SetRenValue: 
//---------------------------------------------------------
static bool SetRenValue( const renValueMap_t *map, const int count, const char *name, GLenum &value )
{
	if ( !StrCheck( name ) ) return false;

	for ( int i = 0; i < count; ++i )
		if ( StrEqual( map[i].name, name ) )
		{
			value = map[i].value;
			return true;
		}
	return false;	
}

//---------------------------------------------------------
// gs_blendMap: 
//---------------------------------------------------------
static const renValueMap_t	gs_blendMap[] =
{
	{ "zero", GL_ZERO },
	{ "one", GL_ONE },
	{ "src_color", GL_SRC_COLOR },
	{ "inv_src_color", GL_ONE_MINUS_SRC_COLOR },
	{ "src_alpha", GL_SRC_ALPHA },
	{ "inv_src_alpha", GL_ONE_MINUS_SRC_ALPHA },
	{ "dst_alpha", GL_DST_ALPHA },
	{ "inv_dst_alpha", GL_ONE_MINUS_DST_ALPHA },
	{ "dst_color", GL_DST_COLOR },
	{ "inv_dst_color", GL_ONE_MINUS_DST_COLOR },
	{ "src_alpha_sat", GL_SRC_ALPHA_SATURATE }
};

static const int gs_blendCount = RENMAP_COUNT( gs_blendMap );

//---------------------------------------------------------
// nsRenState::nsRenState: 
//---------------------------------------------------------
GLShader::GLShader(nsGLProgramsCache &programs) : _programs(programs)
{
	m_zEnable = true;
	m_zWrite = GL_TRUE;
	
	m_alphaTest = false;
	m_alphaRef = 0;
	
	m_alphaBlend = false;
	m_srcBlend = GL_ONE;
	m_dstBlend = GL_ONE;

	m_cullMode = true;
	
	m_texCoordU = GL_REPEAT;
	m_texCoordV = GL_REPEAT;
}

//---------------------------------------------------------
// nsRenState::Reload: 
//---------------------------------------------------------
bool GLShader::Reload()
{
	LogPrintf(PRN_ALL, "...loading shader: %s\n", m_fileName.AsChar());
	nsParseFile	pf;
	script_state_t	*ss = pf.BeginFile( m_fileName );
	if ( !ss )
	{
		LogPrintf( PRN_ALL, "WARNING: can't load shader '%s'\n", m_fileName.AsChar() );
		return false;
	}

	return Parse( ss );
}

//---------------------------------------------------------
// nsRenState::ForceApply: 
//---------------------------------------------------------
void GLShader::ForceApply()
{
	_programs.Bind(_program, false);

	GLUtils::SetState(GL_DEPTH_TEST, m_zEnable);
	glDepthMask(m_zWrite);

	GLUtils::SetState(GL_BLEND, m_alphaBlend);
    glBlendFunc(m_srcBlend, m_dstBlend);

	GLUtils::SetState(GL_CULL_FACE, m_cullMode);

	ApplyTextureParams();

	GL_CHECK_R("GLShader::ForceApply",)
}

//---------------------------------------------------------
// nsRenState::Apply: 
//---------------------------------------------------------
void GLShader::Apply( GLShader *prev )
{
	if ( !prev )
	{
		ForceApply();
		return;
	}

	_programs.Bind(_program, false);

	if ( m_zEnable != prev->m_zEnable )
		GLUtils::SetState(GL_DEPTH_TEST, m_zEnable);
	if ( m_zWrite != prev->m_zWrite )
		glDepthMask(m_zWrite);
	
	if ( m_alphaBlend != prev->m_alphaBlend )
		GLUtils::SetState(GL_BLEND, m_alphaBlend);
	if ( m_srcBlend != prev->m_srcBlend || m_dstBlend != prev->m_dstBlend )
		glBlendFunc(m_srcBlend, m_dstBlend);

	if ( m_cullMode != prev->m_cullMode )
		GLUtils::SetState(GL_CULL_FACE, m_cullMode);

	if ( m_texCoordU != prev->m_texCoordU )
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_texCoordU);
	if ( m_texCoordV != prev->m_texCoordV )
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_texCoordV);

	GL_CHECK_R("GLShader::Apply",)
}

//---------------------------------------------------------
// nsRenState::ReParse: 
//---------------------------------------------------------
bool GLShader::Parse( script_state_t *ss )
{
	const std::string vs = ps_get_str( ss, "vs", nsGLProgramsCache::DEFAULT_VERTEX_SHADER);
	const std::string fs = ps_get_str( ss, "fs", nsGLProgramsCache::DEFAULT_FRAGMENT_SHADER);
	_program = _programs.GetProgram(vs.c_str(), fs.c_str());
	if (!_program) {
		return false;
	}

	std::string str;

	m_zEnable = ps_get_f( ss, "z_enable", 1 ) != 0;
	m_zWrite = ps_get_f( ss, "z_write", 1 ) ? GL_TRUE : GL_FALSE;

	m_alphaTest = ps_get_f( ss, "alpha_test", 0 ) != 0;
	m_alphaRef = ps_get_f( ss, "alpha_ref", 0 );
	if ( m_alphaRef > 255 ) m_alphaRef = 255;
	m_alphaRef /= 255;

	str = ps_get_str( ss, "blend_src", "none" );
	bool res = SetRenValue( gs_blendMap, gs_blendCount, str.c_str(), m_srcBlend );
	
	str = ps_get_str( ss, "blend_dst", "none" );
	res &= SetRenValue( gs_blendMap, gs_blendCount, str.c_str(), m_dstBlend );
	m_alphaBlend = res;

	m_cullMode = ps_get_f( ss, "cull_mode", 1 ) != 0;

	m_texCoordU = StrEqual( ps_get_str( ss, "tex_coord_u", "wrap" ), "clamp" ) ? GL_CLAMP_TO_EDGE : GL_REPEAT;
	m_texCoordV = StrEqual( ps_get_str( ss, "tex_coord_v", "wrap" ), "clamp" ) ? GL_CLAMP_TO_EDGE : GL_REPEAT;

	return true;
}

void GLShader::ApplyTextureParams() const {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_texCoordU);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_texCoordV);
    GL_CHECK_R("glTexParameteri",)
}
