#include "VertexBuffer.h"
#include "D3d_enum.h"
#include "Core/sys.h"
#include "nsLib/log.h"

//-----------------------------------------------------
//  nsVertexBuffer::nsVertexBuffer:  
//-----------------------------------------------------
nsVertexBuffer::nsVertexBuffer( IDirect3DDevice8* dev, uint vertsCount, uint indexCount, bool dynamic )
{
	m_mode = PM_TRIANGLES;
	m_vb = 0;
	m_ib = 0;
	m_verts = 0;
	m_indices = 0;
	prev = next = 0;
	m_dynamic = dynamic;
	ReCreate( dev, vertsCount, indexCount );
}

//-----------------------------------------------------
//  nsVertexBuffer::~nsVertexBuffer:  
//-----------------------------------------------------
nsVertexBuffer::~nsVertexBuffer()
{
	Release();
}

//-----------------------------------------------------
//  nsVertexBuffer::IsValid:  
//-----------------------------------------------------
bool nsVertexBuffer::IsValid()
{
	return m_vb && m_ib && m_vertsCount && m_indexCount && m_verts && m_indices;
}

//-----------------------------------------------------
//  nsVertexBuffer::CreateBuffers:  
//-----------------------------------------------------
bool	nsVertexBuffer::CreateBuffers( IDirect3DDevice8* dev )
{
	HRESULT	hr;
	assert( m_vertsCount > 0 && m_indexCount > 0 );

	DWORD	dyn = m_dynamic ? D3DUSAGE_DYNAMIC : 0;
	
	hr = dev->CreateVertexBuffer( sizeof(vbVertex_t) * m_vertsCount, dyn | D3DUSAGE_WRITEONLY, FVF_VBVERTEX, D3DPOOL_DEFAULT, &m_vb );
	if ( FAILED(hr) )
	{
		LogPrintf( PRN_ALL, "WARNING: Can't create vertex buffer: %s\n", d3d_GetError( hr ) );
		return false;
	}

	hr = dev->CreateIndexBuffer( sizeof(dword) * m_indexCount, dyn | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_ib );
	if ( FAILED(hr) )
	{
		LogPrintf( PRN_ALL, "Can't create index buffer: %s\n", d3d_GetError( hr ) );
		return false;
	}

	/*if ( !nsStates::BeginState( dev ) ) return false;
		dev->SetVertexShader( FVF_VBVERTEX );
		dev->SetStreamSource( 0, m_vb, sizeof(vbVertex_t) );
		dev->SetIndices( m_ib, 0 );
	if ( !nsStates::EndState( dev, m_sbDrawBegin ) ) return false;//*/

	return true;
}

//-----------------------------------------------------
//  nsVertexBuffer::ReCreate:  
//-----------------------------------------------------
bool nsVertexBuffer::ReCreate( IDirect3DDevice8* dev, uint vertsCount, uint indexCount )
{
	Release();
	assert( dev && vertsCount > 0 && indexCount > 0 );

	try
	{
		m_verts = (vbVertex_t*)malloc( sizeof(vbVertex_t) * vertsCount );
		m_vertsCount = m_vertsValidCount = vertsCount;
		for ( uint i = 0; i < vertsCount; i++ )
			m_verts[i].c = 0xffffffff;
		
		m_indices = (word*)malloc( sizeof(word) * indexCount );
		m_indexCount = m_indexValidCount = indexCount;

		if ( !CreateBuffers( dev ) )	throw "Can't create VertexBuffer object!\n";
	}
	catch ( char* error )
	{
		Release();
		Sys_FatalError( error );
		return false;
	}

	return true;
}

//-----------------------------------------------------
//  nsVertexBuffer::Release:  
//-----------------------------------------------------
void nsVertexBuffer::Release()
{
	if ( m_vb ) m_vb->Release(), m_vb = 0;
	if ( m_ib ) m_ib->Release(), m_ib = 0;
	if ( m_verts ) free( m_verts ), m_verts = 0;
	if ( m_indices ) free( m_indices ), m_indices = 0;
	m_vertsCount = 0;
	m_indexCount = 0;
	m_vertsValidCount = 0;
	m_indexValidCount = 0;
	m_isDirtyVB = false;
	m_isDirtyIB = false;
}

//-----------------------------------------------------
//  nsVertexBuffer::OnDeviceLost:  
//-----------------------------------------------------
bool nsVertexBuffer::OnDeviceLost()
{
	if ( m_vb ) m_vb->Release(), m_vb = 0;
	if ( m_ib ) m_ib->Release(), m_ib = 0;
	return true;
}

//-----------------------------------------------------
//  nsVertexBuffer::OnDeviceReset:  
//-----------------------------------------------------
bool nsVertexBuffer::OnDeviceReset( IDirect3DDevice8* dev )
{
	if ( !CreateBuffers( dev ) )
	{
		Sys_FatalError( "Can't reset buffers in VertexBuffer object!" );
		return false;
	}

	m_isDirtyVB = m_isDirtyIB = true;
	return UpdateBuffers();
}

//-----------------------------------------------------
//  nsVertexBuffer::UpdateBuffers:  
//-----------------------------------------------------
bool nsVertexBuffer::UpdateBuffers()
{
	if ( !IsValid() ) return true;
	HRESULT	hr;
	BYTE	*data;

	if ( m_isDirtyVB && m_vertsValidCount )
	{
		hr = m_vb->Lock( 0, 0, &data, 0 );
		if ( FAILED(hr) )
		{
			Sys_FatalError( "Can't load vertex buffer: %s\n", d3d_GetError( hr ) );
			return false;
		}
		memcpy( data, m_verts, sizeof(vbVertex_t) * m_vertsValidCount );
		m_vb->Unlock();
		m_isDirtyVB = false;
	}

	if ( m_isDirtyIB && m_indexValidCount )
	{
		hr = m_ib->Lock( 0, 0, &data, 0 );
		if ( FAILED(hr) )
		{
			Sys_FatalError( "Can't lock index buffer: %s\n", d3d_GetError( hr ) );
			return false;
		}
		memcpy( data, m_indices, sizeof(word) * m_indexValidCount );
		m_ib->Unlock();
		m_isDirtyIB = false;
	}

	return true;
}

void nsVertexBuffer::SetValidVertices( uint count )
{
	if ( count > m_vertsCount )	count = m_vertsCount;
	m_vertsValidCount = count;
}

uint nsVertexBuffer::GetValidVertices()
{
	return m_vertsValidCount;
}
	
void nsVertexBuffer::SetValidIndices( uint count )
{
	if ( count > m_indexCount ) count = m_indexCount;
	m_indexValidCount = count;
}

uint nsVertexBuffer::GetValidIndices()
{
	return m_indexValidCount;
}

void nsVertexBuffer::SetPrimitiveMode( primitiveMode_t mode )
{
	m_mode = mode;
}

uint nsVertexBuffer::GetVerticesCount()
{
	return m_vertsCount;
}

uint nsVertexBuffer::GetIndicesCount()
{
	return m_indexCount;
}

vbVertex_t* nsVertexBuffer::GetReadVertices()
{
	return m_verts;
}

vbVertex_t*	nsVertexBuffer::GetWriteVertices()
{
	m_isDirtyVB = true;
	return m_verts;
}

word* nsVertexBuffer::GetReadIndices()
{
	return m_indices;
}

word* nsVertexBuffer::GetWriteIndices()
{
	m_isDirtyIB = true;
	return m_indices;
}

//-----------------------------------------------------
//  nsVertexBuffer::Draw:  
//-----------------------------------------------------
void nsVertexBuffer::Draw( IDirect3DDevice8* dev )
{
	if ( m_isDirtyIB || m_isDirtyVB )
		if ( !UpdateBuffers() ) return;

	HRESULT	hr;
	D3DPRIMITIVETYPE	primType;
	UINT				primCount;

	if ( m_mode == PM_TRIANGLES )
	{
		primType = D3DPT_TRIANGLELIST;
		primCount = m_indexValidCount / 3;
	}
	else if ( m_mode == PM_TRISTRIPS )
	{
		primType = D3DPT_TRIANGLESTRIP;
		primCount = m_indexValidCount - 2; 
	}
	else if ( m_mode == PM_LINES )
	{
		primType = D3DPT_LINELIST;
		primCount = m_indexValidCount / 2;
	}


	dev->SetVertexShader( FVF_VBVERTEX );
	dev->SetStreamSource( 0, m_vb, sizeof(vbVertex_t) );
	dev->SetIndices( m_ib, 0 );
	
	hr = dev->DrawIndexedPrimitive( primType, 0, m_vertsValidCount, 0, primCount );
#ifdef _DEBUG
	if ( FAILED(hr) )
		LogPrintf( PRN_ALL, "DrawIndexedPrimitive: %s", d3d_GetError( hr ) );
#endif
}
