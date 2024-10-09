#ifndef _VERTEXBUFFER_H_
#define _VERTEXBUFFER_H_

#include "Local.h"

#define	FVF_VBVERTEX	( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

class nsVertexBuffer : public IVertexBuffer
{
friend class nsVertexBufferManager;

public:
	nsVertexBuffer			*prev, *next;	//для создания списка в менеджере

public:
	void	Draw( IDirect3DDevice8* dev );

private:
	IDirect3DVertexBuffer8	*m_vb;
	IDirect3DIndexBuffer8	*m_ib;
	uint					m_vertsCount, m_indexCount;
	uint					m_vertsValidCount, m_indexValidCount;
	bool					m_dynamic;
	
	vbVertex_t				*m_verts;
	word					*m_indices;
	
	bool					m_isDirtyVB;	//true - если данные массиов были изменены и нужно обновить данные буферов
	bool					m_isDirtyIB;
	
	primitiveMode_t			m_mode;

private:
	nsVertexBuffer( IDirect3DDevice8* dev, uint vertsCount, uint indexCount, bool dynamic );
	virtual ~nsVertexBuffer();
	bool	IsValid();
	bool	ReCreate( IDirect3DDevice8* dev, uint vertsCount, uint indexCount );

	void	Release();
	bool	OnDeviceLost();
	bool	OnDeviceReset( IDirect3DDevice8* dev );
	bool	UpdateBuffers();
	bool	CreateBuffers( IDirect3DDevice8* dev );

private:
	virtual void		SetValidVertices( uint count );
	virtual uint		GetValidVertices();
	
	virtual void		SetValidIndices( uint count );
	virtual uint		GetValidIndices();
	
	virtual void		SetPrimitiveMode( primitiveMode_t mode );
	
	virtual uint		GetVerticesCount();
	virtual uint		GetIndicesCount();
	
	virtual vbVertex_t*	GetReadVertices();
	virtual vbVertex_t*	GetWriteVertices();
	
	virtual word*		GetReadIndices();
	virtual word*		GetWriteIndices();
};

#endif
