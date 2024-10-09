// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ALTrackMgr.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "ALTrackMgr.h"
#include "SourceManager.h"
#include "Core/StructUt.h"
#include "nsLib/log.h"

nsALTrackMgr::nsALTrackMgr() :
	m_trackList( 0 )
{

}

//---------------------------------------------------------
// nsALTrackMgr::LoadTrack: 
//---------------------------------------------------------
nsALTrack*	nsALTrackMgr::LoadTrack( const char *fileName )
{
	nsALTrack *track = FindTrack( fileName );
	if ( track )
	{
		track->m_refCount ++;
		return track;
	}

	track = new nsALTrack;
	if ( !track->Load( fileName ) )
	{
		delete track;
		return 0;
	}

	track->m_refCount = 1;
	AddToList( &m_trackList, track );
	return track;
}

//---------------------------------------------------------
// nsALTrackMgr::PlayTrack: 
//---------------------------------------------------------
bool nsALTrackMgr::PlayTrack( nsALTrack *track, nsSource *src )
{
	if ( !track ) return false;

	LogPrintf( PRN_ALL, "play track: '%s'\n", track->GetFileName() );

	if ( track->IsPlaying() )
	{
		LogPrintf( PRN_ALL, "WARNING: track already playing!\n" );
		return false;
	}

	src->BindTrack( track );
	return track->Play( src );
}

//---------------------------------------------------------
// nsALTrackMgr::GetTrack: 
//---------------------------------------------------------
nsALTrack*	nsALTrackMgr::GetTrack( const char *fileName )
{
	nsALTrack	*track = FindTrack( fileName );
	if ( !track )
		track = LoadTrack( fileName );

	return track;
}

//---------------------------------------------------------
// nsALTrackMgr::StopTrack: 
//---------------------------------------------------------
void nsALTrackMgr::StopTrack( nsALTrack *track, bool pause )
{
	track->Stop( pause );
}

//---------------------------------------------------------
// nsALTrackMgr::ReleaseTrack: 
//---------------------------------------------------------
void nsALTrackMgr::ReleaseTrack( nsALTrack *track )
{
	if ( !track ) return;

	track->m_refCount --;

	if ( track->m_refCount <= 0 )
	{
		if ( track->IsPlaying() )
			track->Stop( false );

		DelFromList( &m_trackList, track );
		delete track;
	}
}

//---------------------------------------------------------
// nsALTrackMgr::FindTrack: 
//---------------------------------------------------------
nsALTrack*	nsALTrackMgr::FindTrack( const char *fileName )
{
	nsALTrack	*i;

	for ( i = m_trackList; i; i = i->next )
	{
		if ( StrEqual( fileName, i->GetFileName() ) )
			return i;
	}

	return 0;
}

//---------------------------------------------------------
// nsALTrackMgr::UpdateTracks: 
//---------------------------------------------------------
void nsALTrackMgr::UpdateTracks()
{
	nsALTrack	*i;
	for ( i = m_trackList; i; i = i->next )
		i->UpdatePlaySources();
}

//---------------------------------------------------------
// nsALTrackMgr::ReleaseTracks: 
//---------------------------------------------------------
void nsALTrackMgr::ReleaseTracks()
{
	nsALTrack	*i = m_trackList;
	while ( i )
	{
		nsALTrack	*del = i;
		i = i->next;

		del->Stop( false );
		delete del;
	}
}

