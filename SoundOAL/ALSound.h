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
	nsALSound	*prev, *next;	//��� ��������� (������� ������)

	explicit nsALSound( const char *fileName );
	virtual ~nsALSound();

	bool			Reload();
	bool			Free();		//false - ���� ����� ����� (�������������)

	const char*		GetFileName() const { return m_fileName; }
	ALuint			GetALBuffer() const { return m_alBuff; }
	
private:
	nsString	m_fileName;
	int			m_refCount;
	ALuint		m_alBuff;

	bool	CreateSBFromOgg( const char *fileName, bool sound3d );
	bool	CreateSBFromWav( const char *filename, bool sound3d );
};

#endif //_ALSound_H_