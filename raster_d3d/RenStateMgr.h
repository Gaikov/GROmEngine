// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RenStateMgr.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_RenStateMgr_H_
#define	_RenStateMgr_H_

#include "Local.h"
#include "RenState.h"

class nsRenStateMgr
{
public:
	nsRenStateMgr();
	~nsRenStateMgr();

	nsRenState*	LoadState( const char *fileName );
	void		ReleaseState( nsRenState *state );
	void		ApplyState( IDirect3DDevice8 *dev, nsRenState *state );

private:
	nsRenState	*m_stateList;
	nsRenState	*m_currState;
	nsRenState	m_defState;
};

#endif //_RenStateMgr_H_