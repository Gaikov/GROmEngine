// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RenStateMgr.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "RenStateMgr.h"
#include "Core/StructUt.h"

//---------------------------------------------------------
// nsRenStateMgr::nsRenStateMgr: 
//---------------------------------------------------------
nsRenStateMgr::nsRenStateMgr() :
	m_stateList( 0 ),
	m_currState( 0 )
{
	
}

//---------------------------------------------------------
// nsRenStateMgr::~nsRenStateMgr: 
//---------------------------------------------------------
nsRenStateMgr::~nsRenStateMgr()
{
	nsRenState	*s = m_stateList;
	nsRenState	*del;

	while ( s )
	{
		del = s;
		s = s->next;
		delete del;
	}
}

//---------------------------------------------------------
// nsRenStateMgr::LoadState: 
//---------------------------------------------------------
nsRenState*	nsRenStateMgr::LoadState( const char *fileName )
{
	if ( !StrCheck( fileName ) ) return 0;

	nsRenState	*s;
	for ( s = m_stateList; s; s = s->next )
		if ( StrEqual( s->m_fileName, fileName ) )
			return s;

	s = new nsRenState;
	s->m_fileName = fileName;
	if ( !s->Reload() )
	{
		delete s;
		return 0;
	}
	
	AddToList( &m_stateList, s );
	return s;
}

//---------------------------------------------------------
// nsRenStateMgr::ReleaseState: 
//---------------------------------------------------------
void nsRenStateMgr::ReleaseState( nsRenState *state )
{
	if ( !state ) return;
	DelFromList( &m_stateList, state );
	delete state;
}

//---------------------------------------------------------
// nsRenStateMgr::ApplyState: 
//---------------------------------------------------------
void nsRenStateMgr::ApplyState( IDirect3DDevice8 *dev, nsRenState *state )
{
	if ( !state )
	{
		m_defState.Apply( dev, m_currState );
		m_currState = &m_defState;
	}
	else if ( state != m_currState )
	{
		state->Apply( dev, m_currState );
		m_currState = state;
	}
}