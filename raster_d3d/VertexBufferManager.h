#ifndef _VERTEXBUFFERMANAGER_H_
#define _VERTEXBUFFERMANAGER_H_

#include "Local.h"
#include "VertexBuffer.h"

class nsVertexBufferManager
{
friend class CRenderState;

public:
	nsVertexBufferManager();
	virtual ~nsVertexBufferManager();

	bool			Init( IDirect3DDevice8 *dev );
	void			Release( bool userResource );

private:
	nsVertexBuffer		*m_vbList;
	IDirect3DDevice8	*m_dev;

private:
	nsVertexBuffer*	CreateVertexBuffer( uint vertsCount, uint indexCount, bool dynamic );
	void			ReleaseVertexBuffer( nsVertexBuffer* vb );
};

extern	nsVertexBufferManager	g_vbManager;

#endif
