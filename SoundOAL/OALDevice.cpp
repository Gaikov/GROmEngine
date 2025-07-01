// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file OALDevice.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "OALDevice.h"
#include "Core/Config.h"
#include "nsLib/log.h"
#include "nsLib/FilePath.h"

static nsVar	*snd_volume = nullptr;
static nsVar	*mus_volume = nullptr;

static nsVar	*al_device = nullptr;
nsVar			*al_max_sources = nullptr;

static nsOALDevice g_alDev;

//---------------------------------------------------------
// nsOALDevice::track_play_f: 
//---------------------------------------------------------
void nsOALDevice::track_play_f( int argc, const char *argv[] )
{
	if ( argc < 2 )
	{
		LogPrintf( PRN_ALL, "usage: track_play [track filename]\n" );
		return;
	}

	g_alDev.TestTrack( argv[1] );
}

//---------------------------------------------------------
// nsOALDevice::sound_play_f: 
//---------------------------------------------------------
void nsOALDevice::sound_play_f( int argc, const char *argv[] )
{
	if ( argc < 2 )
	{
		LogPrintf( PRN_ALL, "usage: sound_play [sound filename]\n" );
		return;
	}

	g_alDev.TestSound( argv[1] );
}

//---------------------------------------------------------
// GetSoundDevice: 
//---------------------------------------------------------
ISndDevice*	GetSoundDevice()
{
	return &g_alDev;
}

//---------------------------------------------------------
// nsOALDevice::nsOALDevice: 
//---------------------------------------------------------
nsOALDevice::nsOALDevice() :
	m_aldev(nullptr),
	m_alc(nullptr),
	m_devList(nullptr),
	m_sfxGameVol( 1 ),
	m_musGameVol( 1 )
{

}

//---------------------------------------------------------
// nsOALDevice::SetSFXVolue: 
//---------------------------------------------------------
void nsOALDevice::SetSFXVolume( float vol )
{
	if ( vol <= 0 ) vol = 0;
	if ( vol > 1.0 ) vol = 1.0;

#ifdef _DEBUG
	LogPrintf( PRN_ALL, "sfx volume: %f\n", vol );
#endif
	m_prevSfxVol = vol;

	snd_volume->SetValue( vol );
	m_srcMgr.UpdateVolume( vol, m_sfxGameVol, true );
}

//---------------------------------------------------------
// nsOALDevice::SetMusVolume: 
//---------------------------------------------------------
void nsOALDevice::SetMusVolume( float vol )
{
	if ( vol <= 0 ) vol = 0;
	if ( vol > 1.0 ) vol = 1.0;

#ifdef _DEBUG
	LogPrintf( PRN_ALL, "mus volume: %f\n", vol );
#endif
	
	m_prevMusVol = vol;

	mus_volume->SetValue( vol );
	m_srcMgr.UpdateVolume( vol, m_musGameVol, false );
}

//---------------------------------------------------------
// nsOALDevice::TestTrack: 
//---------------------------------------------------------
void nsOALDevice::TestTrack( const char *fileName )
{
	if ( !fileName || !strlen( fileName ) ) return;
	nsFilePath	name(fileName);
	nsALTrack	*track = m_trkMgr.GetTrack( name );
	if ( !track ) return;

	nsSource *src = m_srcMgr.GetFreeSource();
	if ( !src )
	{
		LogPrintf( PRN_ALL, "WARNING: no free sources for test track\n" );
		return;
	}

	TrackPlay( track );
}

//---------------------------------------------------------
// nsOALDevice::TestSound: 
//---------------------------------------------------------
void nsOALDevice::TestSound( const char *fileName )
{
	if ( !fileName || !strlen( fileName ) ) return;
	nsFilePath	name(fileName);
	nsALSound	*s = m_sndMgr.GetSound( name );

	nsSource *src = m_srcMgr.GetFreeSource();
	if ( !src )
	{
		LogPrintf( PRN_ALL, "WARNING: no free sources for test sound\n" );
		return;
	}
	
	SoundPlay( s, false );
}

//---------------------------------------------------------
// nsOALDevice::Init: 
//---------------------------------------------------------
bool nsOALDevice::Init()
{
	LogPrintf( PRN_ALL,
		"============================================\n" \
		"*            Init Sound Subsystem          *\n" \
		"============================================\n" );

	snd_volume = g_cfg->RegVar( "snd_volume", "0.5", GVF_SAVABLE );
	mus_volume = g_cfg->RegVar( "mus_volume", "0.5", GVF_SAVABLE );
	
	al_device = g_cfg->RegVar( "al_device", "", GVF_SAVABLE );
	al_max_sources = g_cfg->RegVar( "al_max_sources", "32", GVF_SAVABLE );

	g_cfg->RegCmd( "track_play", track_play_f );
	g_cfg->RegCmd( "sound_play", sound_play_f );

#ifdef _DEBUG
	m_devList = AL_EnumDevices();
	AL_PrintDevices( m_devList );

	const	char	*dev_name = AL_MatchDevice( m_devList, al_device->String() );

	//---------- error -------------
	//return false;
	//------------------------------

	if ( dev_name )
		LogPrintf( PRN_ALL, "open AL device: '%s'\n", dev_name );
	else
		LogPrintf( PRN_ALL, "open default AL device: '%s'\n", (char*)alcGetString( 0, ALC_DEFAULT_DEVICE_SPECIFIER ) );
	//if ( !(m_aldev = alcOpenDevice( "Generic Software" )) )//*/
	if ( !(m_aldev = alcOpenDevice( dev_name )) )
	{
		LogPrintf( PRN_ALL, "ERROR: open device: %s\n", ALC_GetError( alcGetError( 0 ) ) );
		return false;
	}
#else
	if ( !(m_aldev = alcOpenDevice(nullptr)) )
	{
		LogPrintf( PRN_ALL, "ERROR: open device: %s\n", ALC_GetError( alcGetError(nullptr) ) );
		return false;
	}
#endif

	LogPrintf( PRN_ALL, "create AL context\n" );
	if ( !(m_alc = alcCreateContext( m_aldev, nullptr)) )
	{
		LogPrintf( PRN_ALL, "ERROR: create context\n" );
		return false;
	}

	LogPrintf( PRN_ALL, "use context\n" );
	if ( !alcMakeContextCurrent( m_alc ) )
	{
		LogPrintf( PRN_ALL, "ERROR: make context current\n" );
		return false;
	}

	LogPrintf( PRN_ALL, "---------- OpenAL info -----------\n" );
	LogPrintf( PRN_ALL, "version: %s\n", alGetString( AL_VERSION ) );
	LogPrintf( PRN_ALL, "renderer: %s\n", alGetString( AL_RENDERER ) );
	LogPrintf( PRN_ALL, "vendor: %s\n", alGetString( AL_VENDOR ) );
	LogPrintf( PRN_ALL, "extensions: %s\n", alGetString( AL_EXTENSIONS ) );
	LogPrintf( PRN_ALL, "----------------------------------\n" );

	m_srcMgr.Init();

	SetSFXVolume( snd_volume->Value() );
	return true;
}

//---------------------------------------------------------
// nsOALDevice::Release: 
//---------------------------------------------------------
void nsOALDevice::Release()
{
	bool	res = true;
	LogPrintf( PRN_ALL, 
		"============================================\n" \
		"*           Release Sound Subsystem        *\n" \
		"============================================\n" );

	m_trkMgr.ReleaseTracks();
	m_srcMgr.Release();
	m_sndMgr.ReleaseAll();

	alcMakeContextCurrent(nullptr);
	if ( m_alc )
	{
		LogPrintf( PRN_ALL, "...destroy context\n" );
		alcDestroyContext( m_alc );
		m_alc = nullptr;
	}

	if ( m_aldev )
	{
		LogPrintf( PRN_ALL, "...close device\n" );
		alcCloseDevice( m_aldev );
		m_aldev = nullptr;
	}

	LogPrintf( PRN_ALL, "\n" );
}

//---------------------------------------------------------
// nsOALDevice::OnActivate: 
//---------------------------------------------------------
void nsOALDevice::OnActivate( bool active )
{
    _active = active;
	if ( !m_alc ) return;	

	if ( active )
	{
		alcProcessContext( m_alc );
	}
	else
	{
		alcSuspendContext( m_alc );
	}	
}

//---------------------------------------------------------
// nsOALDevice::SetListener: 
//---------------------------------------------------------
void nsOALDevice::SetListener( const nsVec3 &pos, const nsVec3 &at, const nsVec3 &up )
{
	if ( !m_alc	) return;

	ALenum	code;
	nsVec3	dir = at - pos;
	dir.Norm();

	ALfloat	ort[] = { dir.x, dir.y, dir.z,	up.x, up.y, up.z };
	alListener3f( AL_POSITION, pos.x, pos.y, pos.z );
	if ( code = alGetError() )
		LogPrintf( PRN_ALL, "set pos: %s\n", AL_GetError( code ) );
	
	alListenerfv( AL_ORIENTATION, ort ); //*/
	if ( code = alGetError() )
		LogPrintf( PRN_ALL, "set ort: %s\n", AL_GetError( code ) );	
}

//---------------------------------------------------------
// nsOALDevice::SetGameVolume: 
//---------------------------------------------------------
void nsOALDevice::SetGameVolume( float sfxVolume, float musVolume )
{
	m_prevSfxVol = -1;	
	m_sfxGameVol = sfxVolume;

	m_prevMusVol = -1;
	m_musGameVol = musVolume;
}

//---------------------------------------------------------
// nsOALDevice::Update: 
//---------------------------------------------------------
void nsOALDevice::Update()
{
	if ( !m_alc || !_active ) return;

	if ( m_prevSfxVol != snd_volume->Value() )
		SetSFXVolume( snd_volume->Value() );

	if ( m_prevMusVol != mus_volume->Value() )
		SetMusVolume( mus_volume->Value() );

	m_trkMgr.UpdateTracks();
}

//---------------------------------------------------------
// nsOALDevice::SoundLoad: 
//---------------------------------------------------------
ISound* nsOALDevice::SoundLoad( const char *fileName )
{
	if ( !m_alc )
	{
		Log::Warning("Can't load: %s", fileName);
		Log::Warning("Sound system is not initialized");
		return nullptr;
	}

	if ( !fileName || !strlen( fileName ) ) return nullptr;
	nsFilePath	name(fileName);
	return m_sndMgr.LoadSound( name );
}

//---------------------------------------------------------
// nsOALDevice::SoundFree: 
//---------------------------------------------------------
void nsOALDevice::SoundFree( ISound *snd )
{
	if ( !snd ) return;

	m_srcMgr.DetachBuffer( (nsALSound*)snd );
	m_sndMgr.ReleaseSound( (nsALSound*)snd );
}

//---------------------------------------------------------
// nsOALDevice::SoundPlay: 
//---------------------------------------------------------
IVoice* nsOALDevice::SoundPlay( ISound *snd, bool loop /*= false */ )
{
	if ( !snd ) return nullptr;
	
	nsSource*	src = m_srcMgr.GetFreeSource();
	auto *s = (nsALSound*)snd;
	if ( !src )
	{
		LogPrintf( PRN_ALL, "WARNING: no free playsound '%s'\n", s->GetFileName() );
		return nullptr;
	}

	ALuint		ps = src->GetSource();
	ALenum		code = 0;
	
	src->m_volume = 1;
	if ( !src->BindBuffer( s ) ) return nullptr;
	src->UpdateVolume( snd_volume->Value(), 1 );
	
	if ( loop )
		alSourcei( ps, AL_LOOPING, AL_TRUE );
	else
		alSourcei( ps, AL_LOOPING, AL_FALSE );
#ifdef _DEBUG
	if ( code = alGetError() )
		LogPrintf( PRN_ALL, "ERROR: alSourcei( AL_LOOPING ): %s\n", AL_GetError( code ) );
#endif

	alSourcef( ps, AL_PITCH, 1 );
	alSourcei( ps, AL_SOURCE_RELATIVE, AL_TRUE );
	alSource3f( ps, AL_POSITION, 0, 0, 0 );
	alSourcePlay( ps );	

	return src;
}

//---------------------------------------------------------
// nsOALDevice::SoundPlay3D: 
//---------------------------------------------------------
IVoice*	nsOALDevice::SoundPlay3D( ISound *snd, const nsVec3 &pos, bool loop, float speedScale )
{
	if ( !snd ) return nullptr;

	nsSource*	src = m_srcMgr.GetFreeSource();
	auto *s = (nsALSound*)snd;
	if ( !src )
	{
		LogPrintf( PRN_ALL, "WARNING: no free playsound '%s'\n", s->GetFileName() );
		return nullptr;
	}

	ALuint		ps = src->GetSource();
	ALenum		code = 0;
	
	if ( !src->BindBuffer( s ) ) return nullptr;

	src->m_volume = 1;
	src->UpdateVolume( snd_volume->Value(), m_sfxGameVol );

	if ( loop )
		alSourcei( ps, AL_LOOPING, AL_TRUE );
	else
		alSourcei( ps, AL_LOOPING, AL_FALSE );
	
	if ( speedScale < 0 ) speedScale = 1;
	alSourcef( ps, AL_PITCH, speedScale );
	alSourcei( ps, AL_SOURCE_RELATIVE, AL_FALSE );
	alSource3f( ps, AL_POSITION, pos.x, pos.y, -pos.z );
	alSourcePlay( ps );

#ifdef _DEBUG
	code = alGetError();
	if ( code )
		LogPrintf( PRN_ALL, "WARNING: play source: %s\n", AL_GetError( code ) );
#endif

	return src;
}

//---------------------------------------------------------
// nsOALDevice::VoiceIsPlaying: 
//---------------------------------------------------------
bool nsOALDevice::VoiceIsPlaying( IVoice *v, ISound *snd /*= 0 */ )
{
	if ( !v ) return false;

	auto *src = (nsSource*)v;

	ALint	state;
	alGetSourcei( src->GetSource(), AL_SOURCE_STATE, &state );

	if ( state == AL_INITIAL || state == AL_STOPPED ) 
		return false;
	
	if ( snd )
	{
		if ( src->GetSound() != (nsALSound*)snd )
			return false;
	}

	return true;
}

//---------------------------------------------------------
// nsOALDevice::VoiceStop: 
//---------------------------------------------------------
void nsOALDevice::VoiceStop( IVoice *v )
{
	if ( !v ) return;
	auto *src = (nsSource*)v;
	alSourceStop( src->GetSource() );
}

//---------------------------------------------------------
// nsOALDevice::VoiceRolloff: 
//---------------------------------------------------------
void nsOALDevice::VoiceRolloff( IVoice *v, float rollOff /*= 1 */ )
{
	if ( !v ) return;
	auto *src = (nsSource*)v;

	alSourcef( src->GetSource(), AL_ROLLOFF_FACTOR, m_srcMgr.GetRollOff() * rollOff );
}

//---------------------------------------------------------
// nsOALDevice::VoiceSetPos: 
//---------------------------------------------------------
void nsOALDevice::VoiceSetPos( IVoice *v, const nsVec3 &pos )
{
	if ( !v ) return;
	auto *src = (nsSource*)v;

	alSource3f( src->GetSource(), AL_POSITION, pos.x, pos.y, -pos.z );
}

//---------------------------------------------------------
// nsOALDevice::VoiceSetVolume: 
//---------------------------------------------------------
void nsOALDevice::VoiceSetVolume( IVoice *v, float volume )
{
	if ( !v ) return;
	auto *src = (nsSource*)v;

	if ( volume < 0 ) volume = 0;
	else if ( volume > 1 ) volume = 1;

	src->m_volume = volume;
	src->UpdateVolume( snd_volume->Value(), m_sfxGameVol );
}

//---------------------------------------------------------
// nsOALDevice::VoiceStopAll: 
//---------------------------------------------------------
void nsOALDevice::VoiceStopAll( bool onlyLooped /*= false */ )
{
	if ( !m_alc ) return;

	m_srcMgr.StopAllSrouces( onlyLooped );
}

//---------------------------------------------------------
// nsOALDevice::VoicePauseAll: 
//---------------------------------------------------------
void nsOALDevice::VoicePauseAll( bool pause, bool onlyLooped /*= false */ )
{
	if ( !m_alc ) return;

	m_srcMgr.PauseAllSources( pause, onlyLooped );
}

//---------------------------------------------------------
// nsOALDevice::TrackLoad: 
//---------------------------------------------------------
ITrack* nsOALDevice::TrackLoad( const char *fileName )
{
	if ( !m_alc ) return nullptr;

	if ( !fileName || !strlen( fileName ) ) return nullptr;

	nsFilePath	name(fileName);
	return m_trkMgr.LoadTrack( name );
}

//---------------------------------------------------------
// nsOALDevice::TrackFree: 
//---------------------------------------------------------
void nsOALDevice::TrackFree( ITrack *t )
{
	if ( !t ) return;

	m_trkMgr.ReleaseTrack( (nsALTrack*)t );
}

//---------------------------------------------------------
// nsOALDevice::TrackPlay: 
//---------------------------------------------------------
void nsOALDevice::TrackPlay( ITrack *t )
{
	if ( !t ) return;

	auto *track = (nsALTrack*)t;

	nsSource	*src = m_srcMgr.GetFreeSource();
	if ( !src )
	{
		LogPrintf( PRN_ALL, "WARNING: no free playtrack '%s'\n", track->GetFileName() );
		return;
	}

	src->m_volume = track->m_volume;
	src->UpdateVolume( mus_volume->Value(), m_musGameVol );
	m_trkMgr.PlayTrack( track, src );
}

//---------------------------------------------------------
// nsOALDevice::TrackIsPlaying: 
//---------------------------------------------------------
bool nsOALDevice::TrackIsPlaying( ITrack *t )
{
	if ( !t ) return false;

	auto *track = (nsALTrack*)t;
	return track->IsPlaying();
}

//---------------------------------------------------------
// nsOALDevice::TrackPause: 
//---------------------------------------------------------
void nsOALDevice::TrackPause( ITrack *t )
{
	if ( !t ) return;
	m_trkMgr.StopTrack( (nsALTrack*)t, true );
}

//---------------------------------------------------------
// nsOALDevice::TrackStop: 
//---------------------------------------------------------
void nsOALDevice::TrackStop( ITrack *t )
{
	if ( !t ) return;
	m_trkMgr.StopTrack( (nsALTrack*)t, false );
}

//---------------------------------------------------------
// nsOALDevice::TrackGetPos: 
//---------------------------------------------------------
uint nsOALDevice::TrackGetPos( ITrack *t )
{
	if ( !t ) return false;

	auto *track = (nsALTrack*)t;
	return track->GetPos();
}

//---------------------------------------------------------
// nsOALDevice::TrackSetPos: 
//---------------------------------------------------------
void nsOALDevice::TrackSetPos( ITrack *t, uint pos )
{
	if ( !t ) return;
	auto *track = (nsALTrack*)t;

	track->SetPos( pos );
}

//---------------------------------------------------------
// nsOALDevice::TrackSetVolume: 
//---------------------------------------------------------
void nsOALDevice::TrackSetVolume( ITrack *t, float vol )
{
	if ( !t ) return;
	auto *track = (nsALTrack*)t;

	if ( vol <= 0 ) vol = 0;
	if ( vol > 1.0 ) vol = 1.0;

	track->m_volume = vol;
	
	if ( track->IsPlaying() )
	{
		nsSource	*src = track->GetSource();
		src->m_volume = vol;
		//LogPrintf( PRN_ALL, "track volume: %f\n" );
		src->UpdateVolume( mus_volume->Value(), m_musGameVol );
	}
}