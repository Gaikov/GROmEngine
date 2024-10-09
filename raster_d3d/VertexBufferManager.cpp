#include "VertexBufferManager.h"
#include "nsLib/log.h"
#include "Core/StructUt.h"

nsVertexBufferManager	g_vbManager;

//---------------------------------------------------------
// nsVertexBufferManager::nsVertexBufferManager:
//---------------------------------------------------------
nsVertexBufferManager::nsVertexBufferManager()
{
	m_vbList = 0;
	m_dev = 0;
}

//---------------------------------------------------------
// nsVertexBufferManager::~nsVertexBufferManager:
//---------------------------------------------------------
nsVertexBufferManager::~nsVertexBufferManager()
{
	m_vbList = 0;
}

//-----------------------------------------------------
//  nsVertexBufferManager::Init:  
//-----------------------------------------------------
bool nsVertexBufferManager::Init( IDirect3DDevice8* dev )
{
	m_dev = dev;
	LogPrintf( PRN_ALL, "...reload vertex buffers\n" );
	nsVertexBuffer	*vb = m_vbList;
	while ( vb )
	{
		if ( !vb->OnDeviceReset( m_dev ) )
			return false;
		vb = vb->next;
	}

	return true;
}

//-----------------------------------------------------
//  nsVertexBufferManager::Release:  
//-----------------------------------------------------
void nsVertexBufferManager::Release( bool userResource )
{
	LogPrintf( PRN_ALL, "...release vertex buffers\n" );
	nsVertexBuffer	*vb = m_vbList;
	while ( vb )
	{
		vb->OnDeviceLost();
		vb = vb->next;
	}

	if ( userResource )
	{
		vb = m_vbList;
		nsVertexBuffer	*del;	
		while ( vb )
		{
			del = vb;
			vb = vb->next;
			delete del;
		}

		m_vbList = 0;
	}
}

//-----------------------------------------------------
//  nsVertexBufferManager::CreateVertexBuffer:  
//-----------------------------------------------------
nsVertexBuffer*	nsVertexBufferManager::CreateVertexBuffer( uint vertsCount, uint indexCount, bool dynamic )
{
	nsVertexBuffer	*vb = new nsVertexBuffer( m_dev, vertsCount, indexCount, dynamic );
	if ( !vb->IsValid() )
	{
		delete vb;
		return 0;
	}

	AddToList( &m_vbList, vb );
	return vb;
}

//-----------------------------------------------------
//  nsVertexBufferManager::ReleaseVertexBuffer:  
//-----------------------------------------------------
void nsVertexBufferManager::ReleaseVertexBuffer( nsVertexBuffer* vb )
{
	DelFromList( &m_vbList, vb );
	delete vb;
}

