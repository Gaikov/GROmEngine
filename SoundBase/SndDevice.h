// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SndDevice.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_SndDevice_H_
#define	_SndDevice_H_

#include "nsLib/Vec3.h"
#include "nsLib/comm_types.h"

struct ISound {};

struct IVoice {};

struct ITrack {};

//---------------------------------------------------------
// ISndDevice: 
//---------------------------------------------------------
struct ISndDevice
{
	virtual bool		Init() = 0;
	virtual void		Release() = 0;

	//должно вызываться при активизации/деактивизации преложения
	virtual void		OnActivate( bool active ) = 0;

	virtual void		SetListener( const nsVec3 &pos, const nsVec3 &at, const nsVec3 &up ) = 0;
	virtual void		SetGameVolume( float sfxVolume, float musVolue ) = 0;

	//должно вызыватся каждый кадр
	virtual void		Update() = 0;

	//звукаовые эффекты
	virtual ISound*		SoundLoad( const char *fileName ) = 0;
	virtual void		SoundFree( ISound *snd ) = 0;
	virtual IVoice*		SoundPlay( ISound *snd, bool loop = false ) = 0;
	virtual IVoice*		SoundPlay3D( ISound *snd, const nsVec3 &pos, bool loop = false, float speedScale = 1 ) = 0;

	virtual bool		VoiceIsPlaying( IVoice *v, ISound *snd = 0 ) = 0;
	virtual void		VoiceStop( IVoice *v ) = 0;
	virtual void		VoiceRolloff( IVoice *v, float rollOff = 1 ) = 0;
	virtual void		VoiceSetPos( IVoice *v, const nsVec3 &pos ) = 0;
	virtual void		VoiceSetVolume( IVoice *v, float volume ) = 0;

	virtual void		VoiceStopAll( bool onlyLooped = false ) = 0;
	virtual void		VoicePauseAll( bool pause, bool onlyLooped = true ) = 0;

	//музыкальные треки
	virtual ITrack*		TrackLoad( const char *fileName ) = 0;
	virtual void		TrackFree( ITrack *t ) = 0;
	virtual void		TrackPlay( ITrack *t ) = 0;
	virtual bool		TrackIsPlaying( ITrack *t ) = 0;
	virtual void		TrackPause( ITrack *t ) = 0;
	virtual void		TrackStop( ITrack *t ) = 0;
	virtual uint		TrackGetPos( ITrack *t ) = 0;
	virtual void		TrackSetPos( ITrack *t, uint pos ) = 0;
	virtual void		TrackSetVolume( ITrack *t, float volume ) = 0;
};

//должно быть реализовано в коде реализации функциональности интерфейса
ISndDevice*	GetSoundDevice();	

#endif //_SndDevice_H_