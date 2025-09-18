// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RenDevice.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_RenDevice_H_
#define	_RenDevice_H_

#include "nsLib/matrix4.h"
#include "nsLib/comm_types.h"

class	nsVar;

/*=========================================
raster configuration desc
=========================================*/
struct rasterConfig_t
{
	uint	defPixelBits;
	uint	currWidth, currHeight;
};

/*-------------------------------------------------------------------------------------
render config variables
-------------------------------------------------------------------------------------*/
extern	nsVar	*r_fullscreen;
extern	nsVar	*r_pixel_bits;
extern	nsVar	*r_tex_bits;
extern	nsVar	*r_vsync;
extern	nsVar	*r_tex_filter;
extern	nsVar	*r_brightness;
extern	nsVar	*r_width;
extern	nsVar	*r_height;
extern	nsVar	*r_refresh;
extern	nsVar	*r_fov;//*/

//texture format GetTexture
typedef enum
{
	TF_RGB,
	TF_RGBA
}
texfmt_t;

#define	CLR_ZBUFF	1u
#define CLR_CBUFF	2u
#define CLR_STENCIL 4u

//DrawChar
typedef struct
{
	float	coord[2];	//in textures
	float	tex_size[2];	//size on texture

	float	size[2];	//real size
	float	offs[2];	//offset in screen
}
rchar_t;

//---------------------------------------------------------
// IRenState: 
//---------------------------------------------------------
struct IRenState
{
	virtual bool IsAlphaTest() = 0;
	virtual bool IsAlphaBlend() = 0;
};

struct IStencilState
{
    virtual ~IStencilState() = default;
    virtual void SetStencilRef(byte value) = 0;
};

struct vbVertex_t
{
	nsVec3	v;
	nsVec3	n;
	dword	c = 0xffffff;
	float tu = 0, tv = 0;

	inline void SetUV(float _u, float _v)
	{
		tu = _u;
		tv = _v;
	}
};

typedef enum
{
	PM_TRIANGLES,
	PM_TRISTRIPS,
	PM_LINES
}
primitiveMode_t;

//---------------------------------------------------------
// IVertexBuffer: ����� ������
//---------------------------------------------------------
struct IVertexBuffer
{
	virtual void		SetValidVertices( uint count ) = 0;
	virtual uint		GetValidVertices() = 0;

	virtual void		SetValidIndices( uint count ) = 0;
	virtual uint		GetValidIndices() = 0;

	virtual void		SetPrimitiveMode( primitiveMode_t mode ) = 0;	//default PM_TRIANGLES

	virtual uint		GetVerticesCount() = 0;
	virtual uint		GetIndicesCount() = 0;

	//������ � ���������
	virtual vbVertex_t*	GetReadVertices() = 0;	//��� ������
	virtual vbVertex_t*	GetWriteVertices() = 0;	//! ������ ��� ������

	//������ � ���������
	virtual word*		GetReadIndices() = 0;	//��� ������
	virtual word*		GetWriteIndices() = 0;	//! ������ ��� ������
};

//---------------------------------------------------------
// ITexture: 
//---------------------------------------------------------
struct ITexture
{
	virtual ~ITexture() = default;
	virtual void GetSize( int &width, int &height ) = 0;
};

struct IRenderTexture : public ITexture {
};

//---------------------------------------------------------
// IRenDevice: 
//---------------------------------------------------------
struct IRenDevice
{
	struct DisplayMode {
		int w = 0, h = 0;
	};
	typedef std::vector<DisplayMode>    tDisplayModes;

	struct DisplayInfo {
		tDisplayModes       modes;
		std::vector<int>    bits;
		//std::vector<int>    frequency;
	};

	//init render
	virtual bool					Init( void *wnd ) = 0;	//�������������, ���������� ����������� ����� ������� �������������
	virtual	void					Release() = 0; //������������, ���������� ����������� ����� �������������
	virtual void                    GetDisplayInfo(DisplayInfo &modes ) = 0;
	virtual const rasterConfig_t*	GetCurrentConfig() = 0;
    virtual void                    InvalidateResources() = 0;

	//������� ���� ����������� ����������
	virtual void			SetColor( const float c[4] ) = 0;

	//textures
	virtual ITexture*		TextureLoad( const char *filename, bool mipmap = true, texfmt_t fmt = TF_RGBA ) = 0;
	virtual ITexture*		TextureGenerate( int width, int height, const void *data, texfmt_t fmt, bool mipmap ) = 0;
	virtual void			TextureRelease( ITexture *texture ) = 0;
	virtual void			TextureBind( ITexture *texture ) = 0;
	virtual void			TextureTranform( const float *offs2, const float *scale2 ) = 0; //in

	virtual IRenderTexture* RenderTextureCreate(int width, int height, texfmt_t = TF_RGBA) = 0;
	virtual void			RenderTextureBind(IRenderTexture *rt) = 0;

	//render state
	virtual IRenState*		StateLoad( const char *fileName ) = 0;
	virtual void			StateRelease( IRenState *state ) = 0;
	virtual void			StateApply( IRenState *state ) = 0;

    virtual IStencilState*  StencilLoad(const char *fileName) = 0;
    virtual void            StencilRelease( IStencilState *state) = 0;
    virtual void            StencilApply( IStencilState *state) = 0;

	//scene functions
	virtual void			ClearScene( uint flags ) = 0;
	virtual bool			BeginScene() = 0;
	virtual	void			EndScene() = 0;

	virtual void			LoadProjMatrix( const float m[16] ) = 0;
	virtual void			LoadViewMartix( const float m[16] ) = 0;

	 //������ �� ���������� ��������, ���������� �� WORLD_MATRIX (OBJECT_MATRIX)
	virtual void			LoadMatrix( const float m[16] ) = 0;
	virtual void			MultMatrixLocal( const float m[16] ) = 0;

	//========= 2d graphics ==========
	//pictures
	virtual void			DrawLinedSprite( float x1, float y1, float x2, float y2, float width, float height ) = 0;
	virtual void			DrawCharScaled( float x, float y, rchar_t *ch, float sx, float sy ) = 0;

	//========= 3d graphics ==========
	virtual IVertexBuffer*  VerticesCreate(uint vertsCount, uint indexCount, bool dynamic = false, bool useColors = false) = 0;
	virtual void			VerticesRelease( IVertexBuffer* vb ) = 0;
	virtual void			VerticesDraw( IVertexBuffer* vb ) = 0;

	//sprites
	virtual void			DrawSprite3D( const nsVec3 &pos, float width, float height, float angle ) = 0;
	virtual void			DrawChar3D( const rchar_t *ch, const nsVec3 &pos, float sx, float sy ) = 0;
	virtual void			DrawPlaneSprite( const nsVec3& p1, const nsVec3& p2, float width, uint wrap_count ) = 0;

	//������� 2-� ���������� �����, �������� ������ � 3D ������
	virtual void			Lighting( bool enable ) = 0;
	virtual void			LightEnable( int source, const nsVec3 &dir, const float c[4] ) = 0;
	virtual void			LightDisable( int source ) = 0;
	virtual void			SetLightAmbient( const float c[4] ) = 0;
};

#endif //_RenDevice_H_