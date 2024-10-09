// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SoundManager.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "SoundManager.h"
#include "Core/StructUt.h"
#include "Core/Package.h"
#include "nsLib/log.h"

//---------------------------------------------------------
// nsSoundManager::nsSoundManager: 
//---------------------------------------------------------
nsSoundManager::nsSoundManager() :
	m_sndList( 0 )
{

}

//---------------------------------------------------------
// nsSoundManager::LoadSound: 
//---------------------------------------------------------
nsALSound* nsSoundManager::LoadSound( const char *fileName )
{
	if ( !fileName || !strlen( fileName ) ) return 0;

	nsString	name = fileName;
	const char		*dot = strchr( name, '.' );
	if ( dot )
		strcpy( (char*)dot, ".ogg" );
	else
		name += ".ogg";
	
	if ( !g_pack.IsExists( name ) )
	{
		const char		*dot = strchr( name, '.' );

		if ( dot )
			strcpy( (char*)dot, ".wav" );
		else
			name += ".wav";
	}
	name.ToLower();
		
	nsALSound	*s = 0;
	LogPrintf( PRN_ALL, "loading sound '%s'\n", name.AsChar() );
	if ( s = FindSound( name ) )
	{
		s->m_refCount ++;
		LogPrintf( PRN_ALL, "found\n" );
	}
	else
	{
		s = new nsALSound( name );
		if ( !s->Reload() )
		{
			delete s;
			s = 0;
		}
		else
		{
			s->m_refCount = 1;
			AddToList( &m_sndList, s );
			LogPrintf( PRN_ALL, "loaded\n" );
		}
	}

	return s;

}

//---------------------------------------------------------
// nsSoundManager::FindSound: 
//---------------------------------------------------------
nsALSound* nsSoundManager::FindSound( const char *fileName )
{
	if ( !fileName || !strlen( fileName ) ) return 0;

	for ( nsALSound	*s = m_sndList; s; s = s->next )
	{
		if ( s->m_fileName == fileName )
			return s;
	}
	return 0;	
}

//---------------------------------------------------------
// nsSoundManager::GetSound: 
//---------------------------------------------------------
nsALSound* nsSoundManager::GetSound( const char *fileName )
{
	nsALSound	*s = FindSound( fileName );
	if ( !s )
		s = LoadSound( fileName );
	return s;
}

//---------------------------------------------------------
// nsSoundManager::ReleaseSound: 
//---------------------------------------------------------
void nsSoundManager::ReleaseSound( nsALSound *s )
{
	if ( !s ) return;

	s->m_refCount --;
	if ( s->m_refCount <= 0 )
	{
		DelFromList( &m_sndList, s );
		delete s;
	}
}

//---------------------------------------------------------
// nsSoundManager::ReleaseAll: 
//---------------------------------------------------------
void nsSoundManager::ReleaseAll()
{
	LogPrintf( PRN_ALL, "...delete buffers\n" );
	nsALSound	*s = m_sndList;
	while ( s )
	{
		nsALSound	*del = s;
		s = s->next;
		LogPrintf( PRN_DEV, "...delete sound: %s\n", del->GetFileName() );
		delete del;
	}
}
