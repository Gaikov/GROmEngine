// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ALTrack.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "ALTrack.h"
#include "OALUtils.h"
#include "SourceManager.h"
#include "nsLib/log.h"


//---------------------------------------------------------
// nsALTrack::nsALTrack: 
//---------------------------------------------------------
nsALTrack::nsALTrack() :
	m_src( 0 ),
	m_freq( 0 ),
	m_fmt( 0 ),
	m_refCount( 0 ),
	m_volume( 1 )
{
	m_alBuff[0] = m_alBuff[1] = m_alBuff[2] = 0;
}

//---------------------------------------------------------
// nsALTrack::~nsALTrack: 
//---------------------------------------------------------
nsALTrack::~nsALTrack()
{
	m_ogg.Free();
}

//---------------------------------------------------------
// nsALTrack::IsPlaying: 
//---------------------------------------------------------
bool nsALTrack::IsPlaying()
{
	if ( m_src )
	{
		ALint	state;
		alGetSourcei( m_src->GetSource(), AL_SOURCE_STATE, &state );

		return ( state != AL_INITIAL && state != AL_STOPPED );
	}

	return false;
}

//---------------------------------------------------------
// nsALTrack::GetPos: 
//---------------------------------------------------------
uint nsALTrack::GetPos()
{
	return m_ogg.Tell();
}

//---------------------------------------------------------
// nsALTrack::SetPos: 
//---------------------------------------------------------
void nsALTrack::SetPos( uint pos )
{
	m_ogg.Seek( (long)pos );
}

//---------------------------------------------------------
// nsALTrack::Load: 
//---------------------------------------------------------
bool nsALTrack::Load( const char *fileName )
{
	if ( !fileName || !strlen( fileName ) ) return false;

	ALenum	code;
	LogPrintf( PRN_ALL, "loading track '%s'\n", fileName );
	
	alGenBuffers( MAX_BUFFERS, m_alBuff );
	if ( code = alGetError() )
	{
		AL_PrintError( "alGenBuffers", code );
		return false;
	}

	if ( !m_ogg.StartReading( fileName, true ) ) return false;

	m_fileName = fileName;
	return true;
}

//---------------------------------------------------------
// nsALTrack::Play: 
//---------------------------------------------------------
bool nsALTrack::Play( nsSource *src )
{
	ALenum	code;

	vorbis_info	*vi = m_ogg.GetVorbisInfo();
    m_fmt = vi->channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	m_freq = vi->rate;

	for ( int i = 0; i < MAX_BUFFERS; i++ )
	{
		uint	size;
		void	*data = GetOGGBuff( size );
		if ( !size ) return false;
		
		alBufferData( m_alBuff[i], m_fmt, data, size, m_freq );
		if ( code = alGetError() )
		{
			AL_PrintError( "alBufferData", code );
			return false;
		}
	}

	m_src = src;
	ALuint	alSrc = m_src->GetSource();
	alSourcei( alSrc, AL_BUFFER, 0 );
	alSourcei( alSrc, AL_LOOPING, AL_FALSE );
	alSourcei( alSrc, AL_SOURCE_RELATIVE, AL_TRUE );

	alSourceQueueBuffers( alSrc, MAX_BUFFERS, m_alBuff );
	if ( code = alGetError() )
	{
		AL_PrintError( "alSourceQueueBuffers", code );
		return false;
	}
	
	alSourcePlay( alSrc );
	if ( code = alGetError() )
	{
		AL_PrintError( "alSourcePlay", code );
		return false;
	}

	return false;
}

//---------------------------------------------------------
// nsALTrack::Stop: 
//---------------------------------------------------------
void nsALTrack::Stop( bool pause )
{
	if ( !m_src ) return;

	ALuint	src = m_src->GetSource();
	ALenum	code;
	
	alSourceStop( src );
	if ( code = alGetError() )
		AL_PrintError( "alSourceStop", code );

	alSourceUnqueueBuffers( src, MAX_BUFFERS, m_alBuff );
	if ( code = alGetError() )
		AL_PrintError( "alSourceUnqueueBuffers", code );//*/

	if ( !pause ) m_ogg.Seek( 0 );

	/*do 
	{
		ALuint	buff;
		alSourceUnqueueBuffers( src, 1, &buff );
		code = alGetError();
	}
	while ( !code );//*/

	//alSourceQueueBuffers()
		
	m_src->BindTrack( 0 );
}

//---------------------------------------------------------
// nsALTrack::UpdatePlaySources: 
//---------------------------------------------------------
void nsALTrack::UpdatePlaySources()
{
	ALenum	code;
	if ( !m_src ) return;
	
	//processing buffers
	ALint	processed;
	ALuint	src = m_src->GetSource();
	alGetSourcei( src, AL_BUFFERS_PROCESSED, &processed );
	if ( code = alGetError() )
	{
		AL_PrintError( "alGetSourcei", code );
		return;
	}

	bool play_src = ( processed == MAX_BUFFERS );

	while ( processed )
	{
		ALuint	buff = 0;
		alSourceUnqueueBuffers( src, 1, &buff );
		//log_printf( PRN_ALL, "buff ret: %i\n", buff );
		if ( code = alGetError() )
		{
			AL_PrintError( "alSourceUnqueueBuffers", code );
			return;
		}

		uint size;
		void *data = GetOGGBuff( size );
		
		if ( data )
		{
			alBufferData( buff, m_fmt, data, size, m_freq );
			if ( code = alGetError() )
			{
				AL_PrintError( "alBufferData", code );
				return;
			}
		}

		alSourceQueueBuffers( src, 1, &buff );
		if ( code = alGetError() )
		{
			AL_PrintError( "alSourceQueueBuffers", code );
			return;
		}

		processed --;
	}

	if ( play_src )
	{
		LogPrintf( PRN_ALL, "WARNING: need more music buffers (curr: %i)\n", MAX_BUFFERS );
		alSourcePlay( src );
	}
}

//---------------------------------------------------------
// nsALTrack::GetOGGBuff: 
//---------------------------------------------------------
void* nsALTrack::GetOGGBuff( uint &size )
{
	static char	buff[READ_SIZE];

	long	read_size = READ_SIZE;
	char	*block = buff;
	while ( read_size )
	{
		long	ret_size = read_size;
		if ( m_ogg.Read( block, ret_size ) == OGG_ERROR )
			return 0;
		block += ret_size;
		read_size -= ret_size;
	}

	size = READ_SIZE;
	return buff;
}
