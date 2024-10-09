// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RenState.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_RenState_H_
#define	_RenState_H_

#include "Local.h"
#include "Core/Parser.h"

class nsRenState : public IRenState
{
public:
	nsRenState	*prev, *next;
	nsString	m_fileName;

public:
	nsRenState();
	bool		Reload();

	void		ForceApply( IDirect3DDevice8 *dev );
	void		Apply( IDirect3DDevice8 *dev, nsRenState *prev );

private:
	DWORD		m_shadeMode;
	DWORD		m_zEnable;
	DWORD		m_zWrite;
	
	BOOL		m_alphaTest;
	DWORD		m_alphaRef;
	
	BOOL		m_alphaBlend;
	DWORD		m_srcBlend;
	DWORD		m_dstBlend;

	DWORD		m_cullMode;
	
	DWORD		m_texCoordU;
	DWORD		m_texCoordV;

private:
	bool			Parse( script_state_t *ss );
	virtual bool	IsAlphaTest() { return m_alphaTest != 0; }
	virtual bool	IsAlphaBlend() { return m_alphaBlend != 0; }
};

#endif //_RenState_H_