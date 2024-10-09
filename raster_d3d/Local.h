#ifndef _D3D_LOCAL_H_
#define _D3D_LOCAL_H_

#include <windows.h>
#include <winerror.h>
#include <d3d8.h>
#include <d3dx8.h>
#include <dxerr8.h>

#include "Core/Memory.h"
#include "nsLib/StrTools.h"
#include "engine/RenDevice.h"

class nsVar;

//Direct3D variables
extern	nsVar	*r_d3d_device;

/*-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------*/
void d3d_FatalError( const char *str_func, HRESULT hr );

//-----------------------------------------------------
#define	COM_RELEASE( c )	if ( c ) c->Release(), c = 0

inline void v3todx ( D3DXVECTOR3 &r, const nsVec3 &v )
{
	r.x = v.x;
	r.y = v.y;
	r.z = v.z;
}

#define	COLOR_TO_D3D( c )	D3DCOLOR_RGBA( int(c[0] * 255.0f), int(c[1] * 255.0f), int(c[2] * 255.0f), int(c[3] * 255.0f) )


/*--------------------------------------------
--------------------------------------------*/
struct color_vert_t
{
	nsVec3	v;
	DWORD	c;
	float	tu, tv;

	inline void Set( const nsVec3 &_v, DWORD _c, float _tu, float _tv )
	{
		v = _v;
		c = _c;
		tu = _tu;
		tv = _tv;
	}
};

#define FVF_COLOR_VERT	(D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE)





#endif