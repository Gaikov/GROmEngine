// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ALSound.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "ALSound.h"
#include "OALUtils.h"
#include "nsLib/log.h"
#include "Core/Memory.h"
#include "SoundBase/OggReader.h"
#include "SoundBase/WavReader.h"

//---------------------------------------------------------
// nsALSound::nsALSound: 
//---------------------------------------------------------
nsALSound::nsALSound( const char *fileName ) :
	m_fileName( fileName ),
	m_refCount( 0 ),
	m_alBuff( 0 )
{
}

//---------------------------------------------------------
// nsALSound::~nsALSound: 
//---------------------------------------------------------
nsALSound::~nsALSound()
{
	
}

//---------------------------------------------------------
// nsALSound::Reload: 
//---------------------------------------------------------
bool nsALSound::Reload()
{
	if ( !m_fileName.Length() ) return false;

	if ( !Free() )
	{
		LogPrintf( PRN_ALL, "WARNING: al buffer is busy!\n" );
		return false;
	}

	if ( strstr( m_fileName, ".ogg" ) )
	{
		if ( !(m_alBuff = CreateSBFromOgg( m_fileName, true )) )
			return false;
	}
	else if ( !(m_alBuff = CreateSBFromWav( m_fileName, true )) )
		return false;

	return true;
}

//---------------------------------------------------------
// nsALSound::Free: 
//---------------------------------------------------------
bool nsALSound::Free()
{
	if ( !m_alBuff ) return true;

	ALenum	code;

	alDeleteBuffers( 1, &m_alBuff );
	if ( code = alGetError() )
	{
		LogPrintf( PRN_ALL, "WARNING: delete buffer: %s\n", AL_GetError( code ) );
		return false;
	}
	else
		m_alBuff = 0;

	return true;
}

//---------------------------------------------------------
// nsALSound::CreateSBFromOgg: 
//---------------------------------------------------------
ALuint nsALSound::CreateSBFromOgg( const char *fileName, bool sound3d )
{
	nsOggReader	ogg;
	ALuint		alBuff = 0;
	void		*buff = 0;
	ALenum		code;
	
	try
	{
		alGenBuffers( 1, &alBuff );
		if ( code = alGetError() ) 
			throw StrPrintf( "ERROR: gen buffer: %s\n", AL_GetError( code ) );

		if ( !ogg.StartReading( fileName, false ) ) throw "";

		long	size;
		buff = ogg.ReadWhole( size );
		if ( !buff ) throw "";

		vorbis_info	*info = ogg.GetVorbisInfo();

		ALenum	format = 0;

		if ( info->channels > 1 )
			format = AL_FORMAT_STEREO16;
		else
			format = AL_FORMAT_MONO16;

		alBufferData( alBuff, format, buff, size, info->rate );
		if ( code = alGetError() )
			throw StrPrintf( "ERROR: copy OGG to buff: %s\n", AL_GetError( code ) );
	}
	catch ( const char *error )
	{
		LogPrintf( PRN_ALL, error );
		if ( alBuff )
		{
			alDeleteBuffers( 1, &alBuff );
			alBuff = 0;
		}
	}
	
	if ( buff )	my_free( buff );
	ogg.Free();
	return alBuff;
}

//---------------------------------------------------------
// nsALSound::CreateSBFromWav: 
//---------------------------------------------------------
ALuint nsALSound::CreateSBFromWav( const char *filename, bool sound3d )
{
	ALuint				al_buff = 0;
	nsWavReader			wav;
	ALenum				code;

	try
	{
		if ( !wav.Read( filename ) ) throw "WARNING: invalid wav file\n";
		auto fmt = wav.GetFormat();
		
		if ( sound3d && fmt->nChannels > 1 ) throw "WARNING: invalid 3D sound!!!\n";
		
		alGenBuffers( 1, &al_buff );
		if ( code = alGetError() ) throw StrPrintf( "ERROR: gen buffer: %s\n", AL_GetError( code ) );

		ALenum	format = 0;
		if ( fmt->nChannels > 1 )
			format = fmt->wBitsPerSample == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
		else
			format = fmt->wBitsPerSample == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;

		alBufferData( al_buff, format, wav.GetSamples(), wav.GetSamplesSize(), fmt->nSamplesPerSec );
		if ( code = alGetError() )
		{
			LogPrintf( PRN_DEV, "\nch: %i\nbits: %i\nhz: %i\n",
				fmt->nChannels,
				fmt->wBitsPerSample,
				fmt->nSamplesPerSec );
			throw StrPrintf( "ERROR: copy WAV to buff: %s\n", AL_GetError( code ) );
		}
	}
	catch ( const char *error )
	{
		LogPrintf( PRN_ALL, error );
		if ( al_buff )
		{
			alDeleteBuffers( 1, &al_buff );
			al_buff = 0;
		}
	}

	return al_buff;
}