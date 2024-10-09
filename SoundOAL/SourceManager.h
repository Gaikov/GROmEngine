// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SourceManager.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_SourceManager_H_
#define	_SourceManager_H_

#include "ALCommon.h"

class nsALSound;
class nsALTrack;

//---------------------------------------------------------
// nsSource: 
//---------------------------------------------------------
class nsSource : public IVoice
{
friend class nsSourceManager;

public:
	nsSource	*prev, *next;	//для списка (не модифицировать!)

	float		m_volume;

public:
	ALuint		GetSource() { return m_src; }
	bool		BindBuffer( nsALSound *snd );
	bool		BindTrack( nsALTrack *snd );
	nsALSound*	GetSound();
	void		UpdateVolume( float globalVol, float gameVol );

private:
	nsSource();

	ALuint		m_src;
	nsALSound	*m_snd;
	nsALTrack	*m_trk;
};

//---------------------------------------------------------
// nsSourceManager: 
//---------------------------------------------------------
class nsSourceManager
{
public:
	nsSourceManager();

	bool		Init();
	void		Release();
	nsSource*	GetFreeSource();
	void		StopAllSrouces( bool onlyLooped );
	void		PauseAllSources( bool pause, bool onlyLooped );
	void		SetRollOff( float val );
	float		GetRollOff() { return m_rollOff; }

	//sfx: true - эффектоу, false - треков
	void		UpdateVolume( float globalVol, float gameVol, bool sfx );
	void		DetachBuffer( nsALSound *snd );
	
private:
	nsSource	*m_srcList;
	int			m_srcCount;
	ALuint		*m_sources;
	float		m_rollOff;
};

#endif //_SourceManager_H_