// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RenState.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "RenState.h"
#include "Core/ParseFile.h"
#include "nsLib/log.h"

struct renValueMap_t
{
	const char	*name;
	DWORD		value;	
};

#define RENMAP_COUNT( array ) sizeof(array) / sizeof(renValueMap_t)

//---------------------------------------------------------
// SetRenValue: 
//---------------------------------------------------------
static bool SetRenValue( const renValueMap_t *map, int count, const char *name, DWORD &value )
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
// gs_shadeMap: 
//---------------------------------------------------------
static const renValueMap_t	gs_shadeMap[] =
{
	{ "flat", D3DSHADE_FLAT },
	{ "gouraud", D3DSHADE_GOURAUD },
	{ "phong", D3DSHADE_PHONG },
};

static const int gs_shadeCount = RENMAP_COUNT( gs_shadeMap );

//---------------------------------------------------------
// gs_blendMap: 
//---------------------------------------------------------
static const renValueMap_t	gs_blendMap[] =
{
	{ "zero", D3DBLEND_ZERO },
	{ "one", D3DBLEND_ONE },
	{ "src_color", D3DBLEND_SRCCOLOR },
	{ "inv_src_color", D3DBLEND_INVSRCCOLOR },
	{ "src_alpha", D3DBLEND_SRCALPHA },
	{ "inv_src_alpha", D3DBLEND_INVSRCALPHA },
	{ "dst_alpha", D3DBLEND_DESTALPHA },
	{ "inv_dst_alpha", D3DBLEND_INVDESTALPHA },
	{ "dst_color", D3DBLEND_DESTCOLOR },
	{ "inv_dst_color", D3DBLEND_INVDESTCOLOR },
	{ "src_alpha_sat", D3DBLEND_SRCALPHASAT }
};

static const int gs_blendCount = RENMAP_COUNT( gs_blendMap );

//---------------------------------------------------------
// nsRenState::nsRenState: 
//---------------------------------------------------------
nsRenState::nsRenState()
{
	m_shadeMode = D3DSHADE_GOURAUD;
	m_zEnable = TRUE;
	m_zWrite = TRUE;
	
	m_alphaTest = FALSE;
	m_alphaRef = 0;
	
	m_alphaBlend = FALSE;
	m_srcBlend = D3DBLEND_ONE;
	m_dstBlend = D3DBLEND_ONE;

	m_cullMode = D3DCULL_CCW;
	
	m_texCoordU = D3DTADDRESS_WRAP;
	m_texCoordV = D3DTADDRESS_WRAP;
}

//---------------------------------------------------------
// nsRenState::Reload: 
//---------------------------------------------------------
bool nsRenState::Reload()
{
	nsParseFile	pf;
	script_state_t	*ss = pf.BeginFile( m_fileName );
	if ( !ss )
	{
		LogPrintf( PRN_ALL, "WARNING: can't load render state '%s'\n", m_fileName.AsChar() );
		return false;
	}

	return Parse( ss );
}

//---------------------------------------------------------
// nsRenState::ForceApply: 
//---------------------------------------------------------
void nsRenState::ForceApply( IDirect3DDevice8 *dev )
{
	dev->SetRenderState( D3DRS_SHADEMODE, m_shadeMode );
	
	dev->SetRenderState( D3DRS_ZENABLE, m_zEnable );
	dev->SetRenderState( D3DRS_ZWRITEENABLE, m_zWrite );
	
	dev->SetRenderState( D3DRS_ALPHATESTENABLE, m_alphaTest );
	dev->SetRenderState( D3DRS_ALPHAREF, m_alphaRef );

	dev->SetRenderState( D3DRS_ALPHABLENDENABLE, m_alphaBlend );
	dev->SetRenderState( D3DRS_SRCBLEND, m_srcBlend );
	dev->SetRenderState( D3DRS_DESTBLEND, m_dstBlend );

	dev->SetRenderState( D3DRS_CULLMODE, m_cullMode );

	dev->SetTextureStageState( 0, D3DTSS_ADDRESSU, m_texCoordU );
	dev->SetTextureStageState( 0, D3DTSS_ADDRESSV, m_texCoordV );
}

//---------------------------------------------------------
// nsRenState::Apply: 
//---------------------------------------------------------
void nsRenState::Apply( IDirect3DDevice8 *dev, nsRenState *prev )
{
	if ( !prev )
	{
		ForceApply( dev );
		return;
	}

	if ( m_shadeMode != prev->m_shadeMode )
		dev->SetRenderState( D3DRS_SHADEMODE, m_shadeMode );
	
	if ( m_zEnable != prev->m_zEnable )
		dev->SetRenderState( D3DRS_ZENABLE, m_zEnable );
	if ( m_zWrite != prev->m_zWrite )
		dev->SetRenderState( D3DRS_ZWRITEENABLE, m_zWrite );
	
	if ( m_alphaTest != prev->m_alphaTest )
		dev->SetRenderState( D3DRS_ALPHATESTENABLE, m_alphaTest );
	if ( m_alphaRef != prev->m_alphaRef )
		dev->SetRenderState( D3DRS_ALPHAREF, m_alphaRef );

	if ( m_alphaBlend != prev->m_alphaBlend )
		dev->SetRenderState( D3DRS_ALPHABLENDENABLE, m_alphaBlend );
	if ( m_srcBlend != prev->m_srcBlend )
		dev->SetRenderState( D3DRS_SRCBLEND, m_srcBlend );
	if ( m_dstBlend != prev->m_dstBlend )
		dev->SetRenderState( D3DRS_DESTBLEND, m_dstBlend );

	if ( m_cullMode != prev->m_cullMode )
		dev->SetRenderState( D3DRS_CULLMODE, m_cullMode );

	if ( m_texCoordU != prev->m_texCoordU )
		dev->SetTextureStageState( 0, D3DTSS_ADDRESSU, m_texCoordU );
	if ( m_texCoordV != prev->m_texCoordV )
		dev->SetTextureStageState( 0, D3DTSS_ADDRESSV, m_texCoordV );
}

//---------------------------------------------------------
// nsRenState::ReParse: 
//---------------------------------------------------------
bool nsRenState::Parse( script_state_t *ss )
{
	const char *str = ps_get_str( ss, "shade_mode", "gouraud" );
	SetRenValue( gs_shadeMap, gs_shadeCount, str, m_shadeMode );

	m_zEnable = ps_get_f( ss, "z_enable", 1 ) ? TRUE : FALSE;
	m_zWrite = ps_get_f( ss, "z_write", 1 ) ? TRUE : FALSE;

	m_alphaTest = ps_get_f( ss, "alpha_test", 0 ) ? TRUE : FALSE;
	m_alphaRef = DWORD( ps_get_f( ss, "alpha_ref", 0 ) );
	if ( m_alphaRef > 255 ) m_alphaRef = 255;

	str = ps_get_str( ss, "blend_src", "none" );
	bool res = SetRenValue( gs_blendMap, gs_blendCount, str, m_srcBlend );
	
	str = ps_get_str( ss, "blend_dst", "none" );
	res &= SetRenValue( gs_blendMap, gs_blendCount, str, m_dstBlend );
	m_alphaBlend = res ? TRUE : FALSE;

	m_cullMode = ps_get_f( ss, "cull_mode", 1 ) ? D3DCULL_CW : D3DCULL_NONE;

	m_texCoordU = StrEqual( ps_get_str( ss, "tex_coord_u", "wrap" ), "clamp" ) ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
	m_texCoordV = StrEqual( ps_get_str( ss, "tex_coord_v", "wrap" ), "clamp" ) ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;

	return true;
}
