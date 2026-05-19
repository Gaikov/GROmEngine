// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SoundManager.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_SoundManager_H_
#define	_SoundManager_H_

#include "ALSound.h"

class nsSoundManager
{
public:
	nsSoundManager();

	nsALSound*	LoadSound( const char *fileName, bool sound3d = true );
	nsALSound*	FindSound( const char *fileName );
	
	//��� ������������
	nsALSound*	GetSound( const char *fileName );
	void		ReleaseSound( nsALSound *s );
	void		ReleaseAll();

private:
	nsALSound	*m_sndList;
};

#endif //_SoundManager_H_