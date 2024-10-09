#ifndef _STATES_H_
#define _STATES_H_

//	Direct3D device state block manager

#include "local.h"
#include "Draw2D.h"

class CStates  
{
protected:
	bool BeginState();				//start write device state block
	bool EndState( DWORD &state );	//end start device state block
	IDirect3DDevice8	*m_pdev;
public:
	void release();

	
	bool init( IDirect3DDevice8 *dev );	//call in CRenderState->init
	CStates();
	virtual ~CStates();
};

extern	CStates	sb;

#endif
