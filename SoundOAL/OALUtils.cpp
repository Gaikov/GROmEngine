// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file OALUtils.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "OALUtils.h"
#include "nsLib/StrTools.h"
#include "nsLib/log.h"

//---------------------------------------------------------
// AL_GetError: 
//---------------------------------------------------------
struct alErrorDesc_t
{
	ALenum		code;
	const char	*errdesc;
};

static const alErrorDesc_t	snd_errlist[] = {
	{ AL_NO_ERROR,  "No Error" },
	{ AL_INVALID_NAME, "Invalid Name parameter" },
	{ AL_INVALID_ENUM, "Invalid parameter." },
	{ AL_INVALID_VALUE, "Invalid enum parameter value." },
	{ AL_INVALID_OPERATION, "Illegal call." },
	{ AL_OUT_OF_MEMORY,  "Unable to allocate memory." }
};

const char* AL_GetError( ALenum code )
{
	for (auto i : snd_errlist)
		if (i.code == code )
			return i.errdesc;
	return "No Error Desc";
}

//---------------------------------------------------------
// ALC_GetError: 
//---------------------------------------------------------
struct alcError_t
{
	ALCenum	code;
	const char	*desc;
};

static const alcError_t alc_errlist[] = 
{
	{ ALC_NO_ERROR, "ALC_NO_ERROR" },
	{ ALC_INVALID_DEVICE, "ALC_INVALID_DEVICE" },
	{ ALC_INVALID_CONTEXT, "ALC_INVALID_CONTEXT" },
	{ ALC_INVALID_ENUM, "ALC_INVALID_ENUM" },
	{ ALC_INVALID_VALUE, "ALC_INVALID_VALUE" }
};

const char*	ALC_GetError( ALCenum code )
{
	for (auto i : alc_errlist)
		if ( code == i.code )
			return i.desc;
	return "No error desc";
}

//---------------------------------------------------------
// AL_EnumDevices: 
//---------------------------------------------------------
const char* AL_EnumDevices()
{
	char *devList = nullptr;

	Log::Info("...enum sound devices" );
	if ( alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT" ) )
	{
		Log::Info("ALC_ENUMERATION_EXT - present!" );
		devList = (char*)alcGetString(nullptr, ALC_DEVICE_SPECIFIER );
		if ( !devList )
		{
			Log::Warning("can't retrieve device list!" );
			return nullptr;
		}
	}
	else
	{
		Log::Warning("'ALC_ENUMERATION_EXT' not present" );
		return nullptr;
	}
	return devList;
}

//---------------------------------------------------------
// Al_PrintDevices: 
//---------------------------------------------------------
void AL_PrintDevices( const char *devList )
{
	if ( !devList ) return;

	Log::Info("-----------------------------------" );
	const char	*list = devList;
	while ( list[0] )
	{
		Log::Info(" - '%s'", list );
		list = strchr( list, 0 );
		list ++;
	}
	Log::Info("-----------------------------------" );
}

//---------------------------------------------------------
// AL_MatchDevice: 
//---------------------------------------------------------
const char* AL_MatchDevice( const char *devList, const char *devName )
{
	if ( !devList || !devName ) return nullptr;

	const char	*list = devList;
	while ( list[0] )
	{
		if ( StrEqual( list, devName ) == 0 )
			return devList;
		
		list = strchr( list, 0 ) + 1;
	}

	return nullptr;
}

//---------------------------------------------------------
// AL_PrintError: 
//---------------------------------------------------------
void AL_PrintError( const char *func, ALenum code )
{
	Log::Info("AL ERROR: %s: %s", func, AL_GetError( code ) );
}