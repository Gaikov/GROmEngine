// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SndManager.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "SndManager.h"

nsSndManager g_sndMgr;
ISndDevice	*g_sndDev = 0;

//---------------------------------------------------------
// nsSndManager::nsSndManager: 
//---------------------------------------------------------
nsSndManager::nsSndManager()
{

}

//---------------------------------------------------------
// nsSndManager::Init: 
//---------------------------------------------------------
bool nsSndManager::Init()
{
	g_sndDev = GetSoundDevice();
	if ( !g_sndDev || !g_sndDev->Init() ) return false;
	
	return true;
}

//---------------------------------------------------------
// nsSndManager::Release: 
//---------------------------------------------------------
void nsSndManager::Release()
{
	if ( g_sndDev )
	{
		g_sndDev->Release();
		g_sndDev = 0;
	}
}

