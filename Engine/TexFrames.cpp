// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file TexFrames.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "TexFrames.h"
#include "nsLib/StrTools.h"
#include "Core/Memory.h"
#include "Core/StructUt.h"
#include "Core/Package.h"
#include "nsLib/log.h"
#include "RenManager.h"

//---------------------------------------------------------
// nsTexFrames::nsTexFrames: 
//---------------------------------------------------------
nsTexFrames::nsTexFrames() :
	m_frame( 0 ),
	m_frameCount( 0 )	
{

}

//---------------------------------------------------------
// nsTexFrames::~nsTexFrames: 
//---------------------------------------------------------
nsTexFrames::~nsTexFrames()
{
	if ( m_frame )
	{
		for ( int i = 0; i < m_frameCount; ++i )
			g_renDev->TextureRelease( m_frame[i] );
		my_free( m_frame );
	}
}

//---------------------------------------------------------
// nsTexFrames::CreateFromEnum: 
//---------------------------------------------------------
nsTexFrames* nsTexFrames::LoadFromEnum( const char *filesName, const char *ext, int digitCount, int startIndex )
{
	if ( !StrCheck( filesName ) 
		&& !StrCheck( ext ) )
		return 0;

	LogPrintf( PRN_ALL, "loading frames: '%s', type: '%s'\n", filesName, ext );

	nsTexFrames	*f = new nsTexFrames;

	ITexture	*tex;

	for ( int i = startIndex;;++i )
	{
		nsString	name;
		name.Format( "%s%s.%s", filesName, StrNumber( i, digitCount ), ext );
		if ( !g_pack.IsExists( name ) )	break;

		tex = g_renDev->TextureLoad( name, true, TF_RGBA );
		if ( !tex ) break;

		AddToArray( &f->m_frame, f->m_frameCount, tex );
	}
	

	if ( !f->m_frameCount )
	{
		delete f;
		f = 0;
		LogPrintf( PRN_ALL, "fail!\n" );
	}
	else
		LogPrintf( PRN_ALL, "%i - frames loaded\n", f->m_frameCount );

	
	return f;
}

//---------------------------------------------------------
// nsTexFrames::ParseFromEnum: 
//---------------------------------------------------------
nsTexFrames* nsTexFrames::ParseFromEnum( script_state_t *ss )
{
	nsString	pattern = ps_get_str( ss, "pattern" );
	nsString	ext = ps_get_str( ss, "ext" );

	int			startIndex = abs( (int)ps_get_f( ss, "start_index" ) );
	int			digCount = abs( (int)ps_get_f( ss, "num_digits" ) );

	return LoadFromEnum( pattern, ext, digCount, startIndex );
}

//---------------------------------------------------------
// nsTexFrames::GetFrame: 
//---------------------------------------------------------
ITexture* nsTexFrames::GetFrame( int idx )
{
	if ( !CheckFrame( idx ) ) return 0;
	return m_frame[idx];
}

//---------------------------------------------------------
// nsTexFrames::CheckFrame: 
//---------------------------------------------------------
bool nsTexFrames::CheckFrame( int idx )
{
	return idx >= 0 && idx < m_frameCount;
}
