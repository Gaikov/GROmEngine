// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SourceManager.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "SourceManager.h"
#include "OALUtils.h"
#include "Core/Config.h"
#include "nsLib/log.h"
#include "ALSound.h"
#include "ALTrack.h"

extern nsVar	*al_max_sources;

//---------------------------------------------------------
// nsSource::nsSource: 
//---------------------------------------------------------
nsSource::nsSource() :
	m_src( 0 ),
	m_snd( 0 ),
	m_trk( 0 ),
	m_volume( 1 )
{
}

//---------------------------------------------------------
// nsSource::BindBuffer: 
//---------------------------------------------------------
bool nsSource::BindBuffer( nsALSound *snd )
{
	ALuint	buff = snd->GetALBuffer();
	if ( !buff )
	{
		LogPrintf( PRN_ALL, "WARNING: sound '%s' not loaded!\n", snd->GetFileName() );
		return false;
	}

	m_snd = snd;
	alSourcei( m_src, AL_BUFFER, buff );

#ifdef _DEBUG
	ALenum	code;
	if ( code = alGetError() )
		LogPrintf( PRN_ALL, "ERROR: alSourcei( AL_BUFFER ): %s\n", AL_GetError( code ) );
#endif

	m_trk = 0;
	return true;
}

//---------------------------------------------------------
// nsSource::BindTrack: 
//---------------------------------------------------------
bool nsSource::BindTrack( nsALTrack *trk )
{
	m_snd = 0;
	m_trk = trk;

	return true;
}

//---------------------------------------------------------
// nsSource::GetSound: 
//---------------------------------------------------------
nsALSound*	nsSource::GetSound()
{
	return m_snd;
}

//---------------------------------------------------------
// nsSource::UpdateVolume: 
//---------------------------------------------------------
void nsSource::UpdateVolume( float globalVol, float gameVol )
{
	ALint	rel;
	alGetSourcei( m_src, AL_SOURCE_RELATIVE, &rel );
	
	float	vol;

	if ( rel )
		vol = globalVol * gameVol * m_volume;
	else
		vol = globalVol * m_volume;
	
	alSourcef( m_src, AL_GAIN, vol );
}

//---------------------------------------------------------
// nsSourceManager::nsSourceManager: 
//---------------------------------------------------------
nsSourceManager::nsSourceManager() :
	m_srcList( 0 ),
	m_srcCount( 0 ),
	m_sources( 0 ),
	m_rollOff( 1 )
{
	
}

//---------------------------------------------------------
// nsSourceManager::Init: 
//---------------------------------------------------------
bool nsSourceManager::Init()
{
	int		maxSrc = al_max_sources->Value() < 4 ? 32 : (int)al_max_sources->Value();
	m_sources = (ALuint*)my_malloc( sizeof(ALuint) * maxSrc );
	while ( 1 )
	{
		alGenSources( maxSrc, m_sources );
		if ( alGetError() )
		{
			if ( maxSrc < 4 )
			{
				LogPrintf( PRN_ALL, "ERROR: can't allocate AL sources\n" );
				return false;
			}
			maxSrc /= 2;
		}
		else
		{
			al_max_sources->SetValue( (float)maxSrc );
			LogPrintf( PRN_ALL, "... %i sources used\n", maxSrc );
			break;
		}
	}

	m_srcCount = maxSrc;
	m_srcList = new nsSource[maxSrc];
	for ( int i = 0; i < m_srcCount; ++i )
		m_srcList[i].m_src = m_sources[i];

	SetRollOff( 10 );
	return true;
}

//---------------------------------------------------------
// nsSourceManager::Release: 
//---------------------------------------------------------
void nsSourceManager::Release()
{
	StopAllSrouces( false );

	if ( m_sources )
	{
		LogPrintf( PRN_ALL, "...free sources\n" );

		alDeleteSources( m_srcCount, m_sources );
		my_free( m_sources );
		m_sources = 0;
	}

	if ( m_srcList )
	{
		delete [] m_srcList;
		m_srcList = 0;
		m_srcCount = 0;
	}
}

//---------------------------------------------------------
// nsSourceManager::GetFreeSource: 
//---------------------------------------------------------
nsSource*	nsSourceManager::GetFreeSource()
{
	if ( !m_srcList) return 0;

	for ( int i = 0; i < m_srcCount; i++ )
	{
		ALint	state;
		if ( m_srcList[i].m_trk ) continue;

		alGetSourcei( m_srcList[i].m_src, AL_SOURCE_STATE, &state );

#ifdef _DEBUG
		ALenum	code;
		if ( code = alGetError() )
		{
			LogPrintf( PRN_ALL, "ERROR: alGetSourcei( AL_SOURCE_STATE ): %s\n", AL_GetError( code ) );
			return 0;
		}
#endif
		if ( state == AL_INITIAL || state == AL_STOPPED )
			return &m_srcList[i];
	}

	return 0;
}

//---------------------------------------------------------
// nsSourceManager::StopAllSrouces: 
//---------------------------------------------------------
void nsSourceManager::StopAllSrouces( bool onlyLooped )
{
	if ( !m_srcList ) return;

	ALint	rel;
	if ( onlyLooped )
	{
		ALint	loop;
		
		for ( int i = 0; i < m_srcCount; i++ )
		{
			alGetSourcei( m_srcList[i].m_src, AL_SOURCE_RELATIVE, &rel );
			if ( !rel )
			{
				alGetSourcei( m_srcList[i].m_src, AL_LOOPING, &loop );
				if ( loop )
					alSourceStop( m_srcList[i].m_src );
			}
		}
	}
	else
	{
		for ( int i = 0; i < m_srcCount; i++ )
		{
			alGetSourcei( m_srcList[i].m_src, AL_SOURCE_RELATIVE, &rel );
			if ( !rel )
				alSourceStop( m_srcList[i].m_src );
		}
	}
}

//---------------------------------------------------------
// nsSourceManager::PauseAllSources: 
//---------------------------------------------------------
void nsSourceManager::PauseAllSources( bool pause, bool onlyLooped )
{
	ALint	loop, state;
	if ( pause )
		LogPrintf( PRN_ALL, "pause sounds\n" );
	else
		LogPrintf( PRN_ALL, "unpause sounds\n" );

	if ( pause )
	{
		for ( int i = 0; i < m_srcCount; i++ )
		{
			if ( m_srcList[i].m_snd )
			{
				if ( onlyLooped )
				{
					alGetSourcei( m_srcList[i].m_src, AL_LOOPING, &loop );
					if ( loop )
						alSourcePause( m_srcList[i].m_src );
				}
				else
					alSourcePause( m_srcList[i].m_src );
			}
		}
	}
	else
	{
		for ( int i = 0; i < m_srcCount; i++ )
		{
			if ( m_srcList[i].m_snd )
			{
				alGetSourcei( m_srcList[i].m_src, AL_SOURCE_STATE, &state );
				if ( onlyLooped )
				{
					alGetSourcei( m_srcList[i].m_src, AL_LOOPING, &loop );
					if ( loop && state == AL_PAUSED )
						alSourcePlay( m_srcList[i].m_src );
				}
				else if ( state == AL_PAUSED )
					alSourcePlay( m_srcList[i].m_src );
			}
		}
	}
}

//---------------------------------------------------------
// nsSourceManager::SetRollOff: 
//---------------------------------------------------------
void nsSourceManager::SetRollOff( float val )
{
	m_rollOff = val;
	for ( int i = 0; i < m_srcCount; i++ )
		alSourcef( m_sources[i], AL_ROLLOFF_FACTOR, val );
}

//---------------------------------------------------------
// nsSourceManager::UpdateVolume: 
//---------------------------------------------------------
void nsSourceManager::UpdateVolume( float globalVol, float gameVol, bool sfx )
{
	for ( int i = 0; i < m_srcCount; ++i )
	{
		if ( (sfx && m_srcList[i].m_snd) 
			|| (!sfx && m_srcList[i].m_trk) )
			m_srcList[i].UpdateVolume( globalVol, gameVol );
	}
}

//---------------------------------------------------------
// nsSourceManager::DetachBuffer: 
//---------------------------------------------------------
void nsSourceManager::DetachBuffer( nsALSound *snd )
{
	for ( int i = 0; i < m_srcCount; i++ )
	{
		if ( m_srcList[i].m_snd == snd )
		{
			alSourceStop( m_srcList[i].m_src );
			alSourcei( m_srcList[i].m_src, AL_BUFFER, 0 );
			m_srcList[i].m_snd = 0;
		}
	}
}

