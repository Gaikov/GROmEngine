#include "States.h"
#include "RenderState.h"
#include "nsLib/log.h"
#include "Core/Config.h"

CStates sb;

CStates::CStates()
{

}

CStates::~CStates()
{

}

bool CStates::init(IDirect3DDevice8 *dev)
{
	LogPrintf( PRN_ALL, "init device states\n" );
	m_pdev = dev;
	
	return true;
}

bool CStates::BeginState()
{
	HRESULT	hr;
	hr = m_pdev->BeginStateBlock(); 
	if ( FAILED(hr) )
	{
		d3d_FatalError( "BeginStateBlock", hr );
		return false;
	}

	return true;
}

bool CStates::EndState(DWORD &state)
{
	HRESULT	hr;
	hr = m_pdev->EndStateBlock( &state ); 
	if ( FAILED(hr) )
	{
		d3d_FatalError( "EndStateBlock", hr );
		return false;
	}

	return true;
}

void CStates::release()
{
	LogPrintf( PRN_ALL, "release device states\n" );
}
