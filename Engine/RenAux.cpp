// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RenAux.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "RenAux.h"
#include "RenManager.h"
#include "Font.h"
#include "nsLib/StrTools.h"
#include "Core/sys.h"
#include "nsLib/log.h"
#include "Engine/GameApp.h"
#include "renderer/sprites/SpriteDesc.h"
#include "renderer/font/FontsCache.h"

static IVertexBuffer	*g_sprBuff = nullptr;
static IVertexBuffer	*g_lineBuff = nullptr;

//---------------------------------------------------------
// RX_Vec2ToVec3: 
//---------------------------------------------------------
inline void RX_Vec2ToVec3( nsVec3 &r, const nsVec2 &v )
{
	r.x = v.x;
	r.y = v.y;
	r.z = 0;
}

//---------------------------------------------------------
// RX_Init: 
//---------------------------------------------------------
bool RX_Init()
{
	//-----------------
	g_sprBuff = g_renDev->VerticesCreate(4, 6, true, false);
	if ( !g_sprBuff )
	{
		Sys_FatalError( "ERROR: can't create sprite vb!\n" );
		return false;
	}

	word	*i = g_sprBuff->GetWriteIndices();
	if ( i )
	{
		word	idx[] = { 0, 1, 2, 0, 2, 3 };
		memcpy( i, idx, sizeof(idx) );
	}
	// 3 |-------| 2
	//   |      /|
	//   |    /  |
	//   |  /    |
	//   |/      |
	// 0 |-------| 1

	//-----------------
	g_lineBuff = g_renDev->VerticesCreate(2, 2, true, false);
	if ( g_lineBuff )
	{
		g_lineBuff->SetPrimitiveMode( PM_LINES );
		i = g_lineBuff->GetWriteIndices();
		i[0] = 0;
		i[1] = 1;
	}
	else
		LogPrintf( PRN_ALL, "WARNING: can't create line vb!\n" );


	return true;
}

//---------------------------------------------------------
// RX_Release: 
//---------------------------------------------------------
void RX_Release()
{
	if ( g_sprBuff )
		g_renDev->VerticesRelease( g_sprBuff );

	if ( g_lineBuff )
		g_renDev->VerticesRelease( g_lineBuff );
}

//---------------------------------------------------------
// RX_DrawFPS: 
//---------------------------------------------------------
void RX_DrawFPS(float x, float y, uint frameTime) {
    static float fps = 0;
    static int frames = 0;
    static uint time = 0;
    frames++;
    time += frameTime;
    if (frames > 60) {
        fps = float(frames) * 1000.0f / float(time);
        frames = 0;
        time = 0;
    }

    auto font = nsFontsCache::Shared()->SysFont();
    font->Draw(
            StrPrintf("%.2f fps", fps),
            nsVec2(x, y),
            nsVec2(0.5f, 1), nsColor::white, 0);
}

//---------------------------------------------------------
// RX_BuildSpriteVerts: 
//---------------------------------------------------------
void RX_BuildSpriteVerts( nsVec2 v[4], const nsVec2 &pos, const nsVec2 &size, const nsVec2 &center, float angle )
{
	if ( angle )
	{
		nsVec2	axisRight( cosf( -angle ), sinf( -angle ) );
		nsVec2	axisUp;
		axisRight.GetLeft( axisUp );

		nsVec2	right = axisRight * size.x;
		nsVec2	up = axisUp * size.y;

		nsVec2	offs = axisRight * center.x + axisUp * center.y;

		v[0] = pos - offs;
		v[1] = pos + right - offs;
		v[2] = pos + right + up - offs;
		v[3] = pos + up - offs;
	}
	else
	{
		v[0] = pos - center;
		v[1] = nsVec2( pos.x + size.x, pos.y ) - center;
		v[2] = nsVec2( pos.x + size.x, pos.y + size.y ) - center;
		v[3] = nsVec2( pos.x, pos.y + size.y ) - center;
	}
}

void	RX_DrawFade( const nsColor &c )
{
	auto game = App_GetGame();
	int w, h;
	game->GetGUIDimension(w, h);

	nsSpriteDesc s;
	s.color = c;
	s.size = nsVec2(w * 2,  h);
    s.pos = nsVec2( w / 2, h / 2);
    s.ComputeCenter();
    s.Draw(g_renDev);
}

//---------------------------------------------------------
// RX_DrawSprite: 
//---------------------------------------------------------
void RX_DrawSprite( const nsSpriteDesc &desc )
{
    desc.Draw(g_renDev);
}

//---------------------------------------------------------
// RX_DrawSpriteLined: 
// ���������� �� ������� pos1 ���� � ������� pos2
// ������������ �������� ������������ �� pos1 � pos2
//---------------------------------------------------------
void RX_DrawStrip( ITexture *tex, const nsVec2 &pos1, const nsVec2 &pos2, float width, const nsColor &color )
{
	g_renDev->TextureBind( tex );

	nsVec2	right, down;

	down = pos2 - pos1;


	down.GetLeft( right );

	float	texY = 1;
	right.Norm();
	right *= width;

	/*int	w = 1, h = 1;
	if ( tex )
		tex->GetSize( w, h );//*/

	nsVec2		pos = pos1 - right * 0.5f;
	vbVertex_t	*v = g_sprBuff->GetWriteVertices();

	RX_Vec2ToVec3( v[0].v, pos + down );
	RX_Vec2ToVec3( v[1].v, pos + down + right );
	RX_Vec2ToVec3( v[2].v, pos + right );
	RX_Vec2ToVec3( v[3].v, pos );

	v[0].tu = 0;
	v[0].tv = 1;

	v[1].tu = 1;
	v[1].tv = 1;

	v[2].tu = 1;
	v[2].tv = 0;

	v[3].tu = 0;
	v[3].tv = 0;

	g_renDev->SetColor(color);

	g_renDev->VerticesDraw( g_sprBuff );
}

//---------------------------------------------------------
// RX_DrawQuad: 
//---------------------------------------------------------
void RX_DrawQuad( ITexture *tex, vbVertex_t quad[4] )
{
	g_renDev->TextureBind( tex );

	vbVertex_t	*v = g_sprBuff->GetWriteVertices();

	for ( int i = 0; i < 4; ++i )
		v[i] = quad[i];

	g_renDev->VerticesDraw( g_sprBuff );
}

//---------------------------------------------------------
// RX_DrawLine: 
//---------------------------------------------------------
void RX_DrawLine( const nsVec2 &v1, const nsVec2 &v2, const nsColor &color )
{
	nsVec3	a, b;
	RX_Vec2ToVec3( a, v1 );
	RX_Vec2ToVec3( b, v2 );

	RX_DrawLine( a, b, color );
}

//---------------------------------------------------------
// RX_DrawLine: 
//---------------------------------------------------------
void RX_DrawLine( const nsVec3 &v1, const nsVec3 &v2, const nsColor &color )
{
	g_renDev->TextureBind( nullptr );

	vbVertex_t	*v = g_lineBuff->GetWriteVertices();

	//v[0].c = v[1].c = color.ToARGB();

	v[0].v = v1;
	v[1].v = v2;

    g_renDev->SetColor(color);
	g_renDev->VerticesDraw( g_lineBuff );
}

//---------------------------------------------------------
// RX_DrawBBox: 
//---------------------------------------------------------
void RX_DrawBBox( const nsVec2 &pos, const nsBBox2 &box, const nsColor &color )
{
	nsVec2	min = pos + box.m_min;
	nsVec2	max = pos + box.m_max;

	RX_DrawLine( min, nsVec2( min.x, max.y ), color );
	RX_DrawLine( min, nsVec2( max.x, min.y ), color );

	RX_DrawLine( nsVec2( min.x, max.y ), max, color );
	RX_DrawLine( nsVec2( max.x, min.y ), max, color );
}

//---------------------------------------------------------
// RX_IndicesStrip: 
//---------------------------------------------------------
void RX_IndicesStrip( IVertexBuffer *vb, int vertCount )
{
	assert( vb );

	word	*idx = vb->GetWriteIndices();
	for ( int i = 0; i < vertCount; ++i )
		idx[i] = i;
}

void RX_DrawSpriteLT(float x, float y, float w, float h, float tu1, float tv1, float tu2, float tv2)
{
	nsVec3 zero;
	dword c = 0xffffffff;
	vbVertex_t v[4] = {
			{nsVec3(x, y, 0), zero, c, tu1, tv1},
			{nsVec3(x, y - h, 0), zero, c, tu1, tv2},
			{nsVec3(x + w, y - h, 0), zero, c, tu2, tv2},
			{nsVec3(x + w, y, 0), zero, c, tu2, tv1},
	};

	vbVertex_t *drawVerts = g_sprBuff->GetWriteVertices();
	for (int i = 0; i < 4; i++)
	{
		drawVerts[i] = v[i];
	}

	g_renDev->VerticesDraw(g_sprBuff);
}

void RX_DrawRect(float const x1, float const y1, float const x2, float const y2, const nsColor &c) {
	g_renDev->TextureBind(nullptr);

	g_renDev->Lighting(false);
	RX_DrawLine(nsVec3(x1, y1, 0), nsVec3(x2, y1, 0), c);
	RX_DrawLine(nsVec3(x1, y2, 0), nsVec3(x2, y2, 0), c);
	RX_DrawLine(nsVec3(x2, y1, 0), nsVec3(x2, y2, 0), c);
	RX_DrawLine(nsVec3(x1, y1, 0), nsVec3(x1, y2, 0), c);
}

void nsGizmos::DrawCross(const nsVec2 &pos, float size, const nsColor &color) {
    g_renDev->TextureBind(nullptr);
    g_renDev->Lighting(false);

    size /= 2;

    nsVec2 x(size, 0);
    nsVec2 y( 0, size);

    RX_DrawLine(pos + x, pos - x, color);
    RX_DrawLine(pos + y, pos - y, color);
}

void nsGizmos::DrawRect(const nsRect &rect, const nsColor &color) {
    RX_DrawRect(rect.minX(), rect.minY(), rect.maxX(), rect.maxY(), color);
}
