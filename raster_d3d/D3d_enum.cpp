#include "Local.h"
#include "D3d_enum.h"
#include "nsLib/log.h"

errdesc_t	errdesc[] = 
{
	{ D3D_OK, "No error occurred. " },
	{ D3DERR_CONFLICTINGRENDERSTATE, "The currently set render states cannot be used together. " },
	{ D3DERR_CONFLICTINGTEXTUREFILTER, "The current texture filters cannot be used together. " },
	{ D3DERR_CONFLICTINGTEXTUREPALETTE, "The current textures cannot be used simultaneously. This generally occurs when a multitexture device requires that all palletized textures simultaneously enabled also share the same palette. " },
	{ D3DERR_DEVICELOST, "The device is lost and cannot be restored at the current time, so rendering is not possible. " },
	{ D3DERR_DEVICENOTRESET, "The device cannot be reset. " },
	{ D3DERR_DRIVERINTERNALERROR, "Internal driver error. " },
	{ D3DERR_INVALIDCALL, "The method call is invalid. For example, a method's parameter may have an invalid value. " },
	{ D3DERR_INVALIDDEVICE, "The requested device type is not valid. " },
	{ D3DERR_MOREDATA, "There is more data available than the specified buffer size can hold. " },
	{ D3DERR_NOTAVAILABLE, "This device does not support the queried technique. " },
	{ D3DERR_NOTFOUND, "The requested item was not found. " },
	{ D3DERR_OUTOFVIDEOMEMORY, "Direct3D does not have enough display memory to perform the operation. " },
	{ D3DERR_TOOMANYOPERATIONS, "The application is requesting more texture-filtering operations than the device supports. " },
	{ D3DERR_UNSUPPORTEDALPHAARG, "The device does not support a specified texture-blending argument for the alpha channel. " },
	{ D3DERR_UNSUPPORTEDALPHAOPERATION, "The device does not support a specified texture-blending operation for the alpha channel. " },
	{ D3DERR_UNSUPPORTEDCOLORARG, "The device does not support a specified texture-blending argument for color values. " },
	{ D3DERR_UNSUPPORTEDCOLOROPERATION, "The device does not support a specified texture-blending operation for color values. " },
	{ D3DERR_UNSUPPORTEDFACTORVALUE, "The device does not support the specified texture factor value. " },
	{ D3DERR_UNSUPPORTEDTEXTUREFILTER, "The device does not support the specified texture filter. " },
	{ D3DERR_WRONGTEXTUREFORMAT, "The pixel format of the texture surface is not valid. " },
	{ E_FAIL, "An undetermined error occurred inside the Direct3D subsystem. " },
	{ E_INVALIDARG, "An invalid parameter was passed to the returning function " },
//	{ E_INVALIDCALL, "The method call is invalid. For example, a method's parameter may have an invalid value. " },
	{ E_OUTOFMEMORY, "Direct3D could not allocate sufficient memory to complete the call. " },
	{ S_OK, "No error occurred. " }
};

int	errcount = sizeof(errdesc) / sizeof(errdesc_t);

fmtdesc_t	fmtdesc[] = 
{
	{ D3DFMT_UNKNOWN, 0, 0, "Default", "Surface format is unknown. " },
	{ D3DFMT_R8G8B8, 32, 3, "R8G8B8", "24-bit RGB pixel format. " },//real 24
	{ D3DFMT_A8R8G8B8, 32, 4, "A8R8G8B8", "32-bit ARGB pixel format with alpha. " },
	{ D3DFMT_X8R8G8B8, 32, 3, "X8R8G8B8", "32-bit RGB pixel format where 8 bits are reserved for each color. " },
		
	{ D3DFMT_A4R4G4B4, 16, 4, "A4R4G4B4", "16-bit ARGB pixel format. " },
	{ D3DFMT_A1R5G5B5, 16, 4, "A1R5G5B5", "16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha (transparent texel). " },
	{ D3DFMT_A8R3G3B2, 16, 4, "A8R3G3B2", "16-bit ARGB texture format. " },

	{ D3DFMT_R5G6B5, 16, 3, "R5G6B5", "16-bit RGB pixel format. " },
	{ D3DFMT_X1R5G5B5, 16, 3, "X1R5G5B5", "16-bit pixel format where 5 bits are reserved for each color. " },
	{ D3DFMT_X4R4G4B4, 16, 3, "X4R4G4B4", "16-bit RGB pixel format where 4 bits are reserved for each color. " },
	
	{ D3DFMT_R3G3B2, 16, 3, "R3G3B2", "8-bit RGB texture format. " },//real 8

	{ D3DFMT_A8, 8, 1, "A8", "8-bit alpha only. " },
	{ D3DFMT_A8P8, 16, 2, "A8P8", "Surface is 8-bit color indexed with 8 bits of alpha. " },
	{ D3DFMT_P8, 8, 1, "P8", "Surface is 8-bit color indexed. " },
	{ D3DFMT_L8, 8, 1, "L8", "8-bit luminance only. " },
	{ D3DFMT_A8L8, 16, 2, "A8L8", "16-bit alpha luminance. " },
	{ D3DFMT_A4L4, 8, 2, "A4L4", "8-bit alpha luminance. " },
	{ D3DFMT_V8U8, 16, 2, "V8U8", "16-bit bump-map format. " },
	{ D3DFMT_L6V5U5, 16, 2, "L6V5U5", "16-bit bump-map format with luminance. " },
	{ D3DFMT_X8L8V8U8, 32, 0, "X8L8V8U8", "32-bit bump-map format with luminance where 8 bits are reserved for each element. " },
	{ D3DFMT_Q8W8V8U8, 32, 0, "Q8W8V8U8", "32-bit bump-map format. " },
	{ D3DFMT_V16U16, 32, 0, "V16U16", "32-bit bump-map format. " },
	{ D3DFMT_W11V11U10, 32, 0, "W11V11U10", "32-bit bump-map format. " },
	{ D3DFMT_UYVY, 0, 0, "UYVY", "UYVY format (PC98 compliance). " },
	{ D3DFMT_YUY2, 0, 0, "YUY2", "YUY2 format (PC98 compliance). " },
	{ D3DFMT_DXT1, 0, 0, "DXT1", "DXT1 compression texture format. " },
	{ D3DFMT_DXT2, 0, 0, "DXT2", "DXT2 compression texture format. " },
	{ D3DFMT_DXT3, 0, 0, "DXT3", "DXT3 compression texture format. " },
	{ D3DFMT_DXT4, 0, 0, "DXT4", "DXT4 compression texture format. " },
	{ D3DFMT_DXT5, 0, 0, "DXT5", "DXT5 compression texture format. " },
	{ D3DFMT_D16_LOCKABLE, 16, 0, "D16_LOCKABLE", "16-bit z-buffer bit depth. This is an application-lockable surface format. " },
	{ D3DFMT_D32, 32, 0, "D32", "32-bit z-buffer bit depth. " },
	{ D3DFMT_D15S1, 16, 0, "D15S1", "16-bit z-buffer bit depth where 15 bits are reserved for the depth channel and 1 bit is reserved for the stencil channel. " },
	{ D3DFMT_D24S8, 32, 0, "D24S8", "32-bit z-buffer bit depth where 24 bits are reserved for the depth channel and 8 bits are reserved for the stencil channel. " },
	{ D3DFMT_D16, 16, 0, "D16", "16-bit z-buffer bit depth. " },
	{ D3DFMT_D24X8, 32, 0, "D24X8", "32-bit z-buffer bit depth where 24 bits are reserved for the depth channel. " },
	{ D3DFMT_D24X4S4, 32, 0, "D24X4S4", "32-bit z-buffer bit depth where 24 bits are reserved for the depth channel and 4 bits are reserved for the stencil channel. " },
	{ D3DFMT_VERTEXDATA, 0, 0, "VERTEXDATA", "Describes a vertex buffer surface. " },
	{ D3DFMT_INDEX16, 16, 0, "INDEX16", "16-bit index buffer bit depth. " },
	{ D3DFMT_INDEX32, 32, 0, "INDEX32", "32-bit index buffer bit depth. " },
};

int	fmtcount = sizeof(fmtdesc) / sizeof(fmtdesc_t);

char* d3d_GetError( HRESULT hr )
{
	/*for ( int i = 0; i < errcount; i++ )
	{
		if ( hr == errdesc[i].code )
			return errdesc[i].desc;
	}//*/
/*	if ( FAILED(hr) )
#if D3D_SDK_VERSION >= 220
	{
		static char str[MAX_STR_LEN];
		D3dXGetErrorString( hr, str, MAX_STR_LEN - 1 );

	}
#else
		return (char*)DXGetErrorString8( hr );
#endif//*/
	return (char*)DXGetErrorString8( hr );

	//return "Invalid d3d error code!!!";
}

/*--------------------------------------------
--------------------------------------------*/

mode_desc_t* CModeArray::FindMode(int width, int height)
{
	if ( !m_pmode ) return 0;
	for ( int i = 0; i < count; i++ )
	{
		if ( m_pmode[i].width == width && m_pmode[i].height == height )
			return &m_pmode[i];
	}

	return 0;
}

bool CModeArray::AddMode(int width, int height, int refresh)
{
	//if ( refresh > 85 ) refresh = 0;
	//refresh = 0;
	mode_desc_t	*m = FindMode( width, height );
	if ( m )
	{
		if ( m->refresh < refresh )
			m->refresh = refresh;
		return true;
	}
	else
	{
		if ( !m_pmode )
			m_pmode = (mode_desc_t*)my_malloc( sizeof(mode_desc_t) );
		else
			m_pmode = (mode_desc_t*)my_realloc( m_pmode, sizeof(mode_desc_t) * (count + 1) );
		if ( !m_pmode ) return false;

		m_pmode[count].width = width;
		m_pmode[count].height = height;
		m_pmode[count].refresh = refresh;

		count ++;

/*		if ( !m_pmode[0].width )
		{
			int	i = 100;
			i ++;
		}//*/

		return true;
	}

	return false;
}

mode_desc_t* CModeArray::GetMode(int index)
{
	if ( index < 0 || index >= count ) return 0;
	return &m_pmode[index];
}

int CModeArray::FindUserMode( int width, int height )
{
	int by_width = -1;

	for ( int i = 0; i < count; i++ )
	{
		if ( width == m_pmode[i].width )
		{
			if ( by_width < 0 ) by_width = i;
			if ( height == m_pmode[i].height )
				return i;
		}
	}

	return by_width;
}

//------------------------------------
// d3d_EnumModes: 
//------------------------------------
bool d3d_EnumModes( CModeArray &modes, UINT dev, IDirect3D8 *d3d )
{
	int	count = 0;
	D3DDISPLAYMODE	m;
		
	LogPrintf( PRN_DEV, "...enum modes\n" );
	while ( d3d->EnumAdapterModes( dev, count, &m ) == D3D_OK )
	{
		modes.AddMode( m.Width, m.Height, m.RefreshRate );


		count ++;
	}

	return count != 0;
}

//------------------------------------
// d3d_BackBufferFormat: 
//------------------------------------
D3DFORMAT	d3d_BackBufferFormat( UINT dev, D3DDEVTYPE type, int depth, IDirect3D8 *d3d )
{
	HRESULT	hr;
	for ( int i = 0; i < fmtcount; i++ )
		if ( fmtdesc[i].bits == depth )
		{
			hr = d3d->CheckDeviceType( dev, type, fmtdesc[i].fmt, fmtdesc[i].fmt, FALSE );
			if ( SUCCEEDED(hr) ) return fmtdesc[i].fmt;
		}
	
	return D3DFMT_UNKNOWN;
}

//------------------------------------
// d3d_WinBackBufferFormat: 
//------------------------------------
D3DFORMAT d3d_WinBackBufferFormat( UINT dev, D3DDEVTYPE type, D3DFORMAT deskfmt, IDirect3D8 *d3d )
{
	HRESULT	hr;
	/*for ( int i = 0; i < fmtcount; i++ )
	{
		hr = d3d->CheckDeviceType( dev, type, deskfmt, fmtdesc[i].fmt, TRUE );
		if ( SUCCEEDED(hr) ) return fmtdesc[i].fmt;
	}//*/

	hr = d3d->CheckDeviceType( dev, type, deskfmt, deskfmt, TRUE );
	if ( SUCCEEDED(hr) ) return deskfmt;

	return D3DFMT_UNKNOWN;
}

//------------------------------------
// d3d_GetTextureFormat: 
//------------------------------------
D3DFORMAT	d3d_GetTextureFormat( int texbits, D3DDEVTYPE type, D3DFORMAT backbuff, IDirect3D8 *d3d )
{
	for ( int f = 0; f < fmtcount; f++ )
	{
		if ( texbits == fmtdesc[f].bits )
			if ( d3d->CheckDeviceFormat( 0, type, backbuff, 0, D3DRTYPE_TEXTURE, fmtdesc[f].fmt ) == D3D_OK )
				return fmtdesc[f].fmt;
	}

	return D3DFMT_UNKNOWN;
}

//------------------------------------
// d3d_GetTextureFormatRGB: 
//------------------------------------
D3DFORMAT	d3d_GetTextureFormatRGB( int texbits, D3DDEVTYPE type, D3DFORMAT backbuff, IDirect3D8 *d3d )
{
	for ( int f = 0; f < fmtcount; f++ )
	{
		if ( texbits == fmtdesc[f].bits && fmtdesc[f].components == 3 )
			if ( d3d->CheckDeviceFormat( 0, type, backbuff, 0, D3DRTYPE_TEXTURE, fmtdesc[f].fmt ) == D3D_OK )
				return fmtdesc[f].fmt;
	}

	return d3d_GetTextureFormat( texbits, type, backbuff, d3d );
}

//------------------------------------
// d3d_GetFormatDesc: 
//------------------------------------
fmtdesc_t*	d3d_GetFormatDesc( D3DFORMAT fmt )
{
	for ( int i = 0; i < fmtcount; i++ )
		if ( fmtdesc[i].fmt == fmt )
			return &fmtdesc[i];
	return 0;
}