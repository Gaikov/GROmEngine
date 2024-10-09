// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Anim.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "Anim.h"
#include "nsLib/Vec2.h"
#include "Core/StructUt.h"
#include "Core/ScriptSaver.h"
#include "nsLib/log.h"
#include "Core/Package.h"
#include "TimeFormat.h"

//---------------------------------------------------------
// nsTexAnimState::nsTexAnimState: 
//---------------------------------------------------------
nsAnimState::nsAnimState() :
	m_frame( 0 ),
	m_currSet( 0 ),
	m_currAnim( 0 )
{

}

//---------------------------------------------------------
// nsTexAnim::nsTexAnim: 
//---------------------------------------------------------
nsAnim::nsAnim() :
	m_frame( 0 ),
	m_frameCount( 0 )	
{

}

//---------------------------------------------------------
// nsTexAnim::~nsTexAnim: 
//---------------------------------------------------------
nsAnim::~nsAnim()
{
	if ( m_frame )
		my_free( m_frame );
}

//---------------------------------------------------------
// nsTexAnim::Load: 
//---------------------------------------------------------
nsAnim* nsAnim::Load( const char *fileName )
{
	if ( !StrCheck( fileName ) ) return 0;

	nsFile	*file = g_pack.LoadFile( fileName );
	if ( !file ) return 0;

	script_state_t	*ss = ps_begin( (char*)file->GetData() );

	nsAnim	*anim = new nsAnim;
	if ( !anim->Parse( ss ) )
	{
		delete anim;
		anim = 0;
	}

	ps_end( ss );
	g_pack.ReleaseFile( file );
	return anim;
}

//---------------------------------------------------------
// nsTexAnim::MakePattern: 
//---------------------------------------------------------
void nsAnim::anim_make_f( int argc, const char *argv[] )
{
	if ( argc < 2 )
	{
		LogPrintf( PRN_ALL, "usage: anim_make [filename] [framesets count]\n" );
		return;
	}

	nsScriptSaver	s( argv[1] );

	int	count = 0;
	if ( argc >= 3 ) count = abs( atoi( argv[2] ) );
	if ( !count ) count = 2;

	for ( int i = 0; i < count; i++ )
	{
		s.BlockBegin( "$frameset" );
		{
			s.Printf( "$id \"name%03i\"", i );
			s.Printf( "$fps 30" );
			s.Printf( "$range %i %i", i * 10, i * 10 + 9 );
			s.Printf( "$loop 0" );
			s.Printf( "$next \"\"" );
		}
		s.BlockEnd();
	}

	LogPrintf( PRN_ALL, "Done!\n" );
}

//---------------------------------------------------------
// nsTexAnim::AnimPlay: 
//---------------------------------------------------------
void nsAnim::AnimPlay( const nsName &name, nsAnimState &state )
{
	if ( AnimIsPlaying( name, state ) ) return;
	
	nsAnimFrames	*fs = FindFrameset( name );
	if ( !fs )
	{
		LogPrintf( PRN_ALL, "WARNING: '%s' frameset not found!\n", name.AsChar() );
		return;
	}

	state.m_currAnim = this;
	state.m_frame = (float)fs->range[0];
	state.m_currSet = fs;
}

//---------------------------------------------------------
// nsAnim::AnimMove: 
//---------------------------------------------------------
void nsAnim::AnimMove( nsAnimState &state )
{
	if ( state.m_currAnim != this || !state.m_currSet ) return;

	nsAnimFrames	*fs = state.m_currSet;

	state.m_frame += fs->fps * g_frameTime;
	if ( state.m_frame >= fs->range[1] )
	{
		if ( fs->loop )
			state.m_frame = state.m_frame - fs->range[1] + fs->range[0];
		else
			state.m_frame = (float)fs->range[1];
	}
}

//---------------------------------------------------------
// nsAnim::AnimIsPlaying: 
//---------------------------------------------------------
bool nsAnim::AnimIsPlaying( const nsName &name, nsAnimState &state )
{
	return ( state.m_currAnim == this
		&& state.m_currSet
		&& state.m_currSet->id == name );
}

//---------------------------------------------------------
// nsTexAnim::Parse: 
//---------------------------------------------------------
bool nsAnim::Parse( script_state_t *ss )
{
	//--------------------------------------------
	if ( ps_block_begin( ss, "frameset" ) )
	{
		do
		{
			nsAnimFrames	fs;

			fs.id = nsName( ps_get_str( ss, "id" ) );
			fs.fps = ps_get_f( ss, "fps" );
			
			float	range[2];
			ps_get_2f( ss, "range", range, nsVec2( 0, 0 ) );
			fs.range[0] = (int)range[0];
			fs.range[1] = (int)range[1];
			
			fs.loop = ps_get_f( ss, "loop" ) ? true : false;
			fs.next = nsName( ps_get_str( ss, "next" ) );

			AddToArray( &m_frame, m_frameCount, fs );
		}
		while( ps_block_next( ss ) );
		ps_block_end( ss );
	}
	else
		LogPrintf( PRN_ALL, "WARNING: frameset not found!\n" );
	//--------------------------------------------

	return m_frameCount != 0;
}

//---------------------------------------------------------
// nsAnim::FindFrameset: 
//---------------------------------------------------------
nsAnimFrames* nsAnim::FindFrameset( const nsName &name )
{
	for ( int i = 0; i < m_frameCount; ++i )
		if ( m_frame[i].id == name )
			return &m_frame[i];
	return 0;	
}


