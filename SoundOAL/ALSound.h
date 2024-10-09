// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ALSound.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_ALSound_H_
#define	_ALSound_H_

#include "ALCommon.h"
#include "nsLib/StrTools.h"

class nsALSound : public ISound
{
friend class nsSoundManager;

public:
	nsALSound	*prev, *next;	//для менеджера (хранить список)

public:
	nsALSound( const char *fileName );
	virtual ~nsALSound();

	bool			Reload();
	bool			Free();		//false - если буфер занят (проигрывается)

	static ALuint	CreateSBFromOgg( const char *fileName, bool sound3d );
	static ALuint	CreateSBFromWav( const char *filename, bool sound3d );

	const char*		GetFileName() { return m_fileName; }
	ALuint			GetALBuffer() { return m_alBuff; }
	
private:
	nsString	m_fileName;
	int			m_refCount;
	ALuint		m_alBuff;
};

#endif //_ALSound_H_