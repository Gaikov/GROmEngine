// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file OALUtils.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_OALUtils_H_
#define	_OALUtils_H_

#include "ALCommon.h"

const char* AL_GetError( ALenum code );
const char*	ALC_GetError( ALCenum code );

//возврадает список строк рахделенных \0 и завершенных \0\0
const char* AL_EnumDevices();
void		AL_PrintDevices( const char *devList );
const char* AL_MatchDevice( const char *devList, const char *devName );

void		AL_PrintError( const char *func, ALenum code );

#endif //_OALUtils_H_