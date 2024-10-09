// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file OALDevice.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_OALDevice_H_
#define	_OALDevice_H_

#include "OALUtils.h"
#include "SoundManager.h"
#include "SourceManager.h"
#include "ALTrackMgr.h"

class nsOALDevice : public ISndDevice
{
public:
	nsOALDevice();

private:
	ALCdevice			*m_aldev;
	ALCcontext			*m_alc;

	const char			*m_devList;

	nsSoundManager		m_sndMgr;
	nsSourceManager		m_srcMgr;
	nsALTrackMgr		m_trkMgr;

	float				m_sfxGameVol;
	float				m_musGameVol;

	float				m_prevSfxVol;
	float				m_prevMusVol;
    bool                _active = true;

private:
	static void			track_play_f( int argc, const char *argv[] );
	static void			sound_play_f( int argc, const char *argv[] );

	void				SetSFXVolume( float vol );
	void				SetMusVolume( float vol );
	void				TestTrack( const char *fileName );
	void				TestSound( const char *fileName );

private:
	virtual bool		Init();
	virtual void		Release();

	//������ ���������� ��� �����������/������������� ����������
	virtual void		OnActivate( bool active );

	virtual void		SetListener( const nsVec3 &pos, const nsVec3 &at, const nsVec3 &up );
	virtual void		SetGameVolume( float sfxVolume, float musVolue );

	//������ ��������� ������ ����
	virtual void		Update();

	//��������� �������
	virtual ISound*		SoundLoad( const char *fileName );
	virtual void		SoundFree( ISound *snd );
	virtual IVoice*		SoundPlay( ISound *snd, bool loop = false );
	virtual IVoice*		SoundPlay3D( ISound *snd, const nsVec3 &pos, bool loop = false, float speedScale = 1 );

	virtual bool		VoiceIsPlaying( IVoice *v, ISound *snd = 0 );
	virtual void		VoiceStop( IVoice *v );
	virtual void		VoiceRolloff( IVoice *v, float rollOff = 1 );
	virtual void		VoiceSetPos( IVoice *v, const nsVec3 &pos );
	virtual void		VoiceSetVolume( IVoice *v, float volume );

	virtual void		VoiceStopAll( bool onlyLooped = false );
	virtual void		VoicePauseAll( bool pause, bool onlyLooped = false );

	//����������� �����
	virtual ITrack*		TrackLoad( const char *fileName );
	virtual void		TrackFree( ITrack *t );
	virtual void		TrackPlay( ITrack *t );
	virtual bool		TrackIsPlaying( ITrack *t );
	virtual void		TrackPause( ITrack *t );
	virtual void		TrackStop( ITrack *t );
	virtual uint		TrackGetPos( ITrack *t );
	virtual void		TrackSetPos( ITrack *t, uint pos );
	virtual void		TrackSetVolume( ITrack *t, float volume );
};

#endif //_OALDevice_H_