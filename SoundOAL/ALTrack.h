// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ALTrack.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_ALTrack_H_
#define	_ALTrack_H_

#include "nsLib/StrTools.h"
#include "ALCommon.h"
#include "SoundBase/OggReader.h"

class nsSource;

//---------------------------------------------------------
// nsALTrack: 
//---------------------------------------------------------
class nsALTrack : public ITrack
{
friend class nsALTrackMgr;

public:
	nsALTrack	*prev, *next;

	float		m_volume;

public:
	const char* GetFileName() { return m_fileName; }
	bool		IsPlaying();
	uint		GetPos();
	void		SetPos( uint pos );
	nsSource*	GetSource() { return m_src; }

private:
	enum
	{
		MAX_BUFFERS = 3,
		READ_SIZE = 32768
	};

	nsString	m_fileName;
	nsOggReader	m_ogg;

	ALuint		m_alBuff[MAX_BUFFERS];
	nsSource	*m_src;

	ALsizei		m_freq;
	ALenum		m_fmt;

	int			m_refCount;
	//bool		m_pause;

private:
public:
	nsALTrack();
	virtual ~nsALTrack();

	bool		Load( const char *fileName );
	bool		Play( nsSource *src );
	void		Stop( bool pause );
	void		UpdatePlaySources();
	void*		GetOGGBuff( uint &size );
};

#endif //_ALTrack_H_