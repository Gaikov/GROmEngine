// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SndManager.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_SndManager_H_
#define	_SndManager_H_

#include "SoundBase/SndDevice.h"

class nsSndManager
{
public:
	nsSndManager();

	bool	Init();
	void	Release();

private:

};

extern nsSndManager g_sndMgr;
extern ISndDevice	*g_sndDev;

#endif //_SndManager_H_