// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ALTrackMgr.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_ALTrackMgr_H_
#define	_ALTrackMgr_H_

#include "ALCommon.h"
#include "ALTrack.h"

class nsALTrackMgr  
{
public:
	nsALTrackMgr();

	nsALTrack*	LoadTrack( const char *fileName );
	bool		PlayTrack( nsALTrack *track, nsSource *src );
	
	//для тестирования
	nsALTrack*	GetTrack( const char *fileName );
	
	void		StopTrack( nsALTrack *track, bool pause );
	void		ReleaseTrack( nsALTrack *track );
	
	void		UpdateTracks();
	void		ReleaseTracks();

private:
	nsALTrack	*m_trackList;

private:
	nsALTrack*	FindTrack( const char *fileName );
};

#endif //_ALTrackMgr_H_