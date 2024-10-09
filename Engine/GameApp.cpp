// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GameApp.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "GameApp.h"

bool g_sysPaused = false;

//---------------------------------------------------------
// pause_f: 
//---------------------------------------------------------
void pause_f( int argc, const char *argv[] )
{
	App_SetPause( !g_sysPaused );
}

//---------------------------------------------------------
// App_IsPaused: 
//---------------------------------------------------------
bool App_IsPaused()
{
	return g_sysPaused;
}

//---------------------------------------------------------
// App_SetPause: 
//---------------------------------------------------------
void App_SetPause( bool pause )
{
	if ( pause == g_sysPaused ) return;

	g_sysPaused = pause;
	App_GetGame()->OnPause( g_sysPaused );
}
