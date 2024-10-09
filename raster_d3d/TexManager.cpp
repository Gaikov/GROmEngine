#include "RenderState.h"
#include "States.h"
#include "nsLib/StrTools.h"
#include "nsLib/log.h"
#include "Core/Package.h"
#include "Core/Config.h"

//---------------------------------------------------------
// nsTexture::nsTexture: 
//---------------------------------------------------------
nsTexture::nsTexture() :
	next( 0 ),
	prev( 0 ),
	data( 0 ),
	tex( 0 )
{

}

//---------------------------------------------------------
// nsTexture::GetSize: 
//---------------------------------------------------------
void nsTexture::GetSize( int &w, int &h )
{
	w = width;
	h = height;
}

typedef struct
{
	uchar	b, g, r, a;
}
fmt_argb_t;

typedef struct
{
	uchar	r, g, b, a;
}
rgba_t;

CTexManager	tm;

//---------------------------------------------------------
// CTexManager::tm_reload_textures_f: 
//---------------------------------------------------------
void CTexManager::tm_reload_textures_f( int argc, const char *argv[] )
{
	nsTexture	*tex = tm.m_texList;
	while ( tex )
	{
		if ( tex->tex )
		{
			LogPrintf( PRN_ALL, "reload: '%s'\n", tex->m_fileName.AsChar() );
			tex->tex->Release();
			tex->tex = 0;
			tm.ReloadTexture( tex );
		}

		tex = tex->next;
	}
	LogPrintf( PRN_ALL, "DONE!\n" );
}

//---------------------------------------------------------
// CTexManager::tm_reload_texture_f: 
//---------------------------------------------------------
void CTexManager::tm_reload_texture_f( int argc, const char *argv[] )
{
	if ( argc < 2 )
	{
		LogPrintf( PRN_ALL, "usage: reload_texture [bitmap filename]\n" );
		return;
	}

	nsTexture *tex = tm.m_texList;
	while ( tex )
	{
		if ( tex->m_fileName == argv[1] )
		{
			if ( tex->tex )
			{
				tex->tex->Release();
				tex->tex = 0;
			}

			tm.ReloadTexture( tex );
			LogPrintf( PRN_ALL, "DONE!\n" );
			return;
		}

		tex = tex->next;
	}
	LogPrintf( PRN_ALL, "texture '%s' not found\n", argv[1] );
}

//---------------------------------------------------------
// CTexManager::tm_list_f: 
//---------------------------------------------------------
void CTexManager::tm_list_f( int argc, const char *argv[] )
{
	LogPrintf( PRN_ALL, "========= loaded textures =========\n" );
	nsTexture *tex = tm.m_texList;
	while ( tex )
	{
		if ( tex->bitmap )
			LogPrintf( PRN_ALL, "[ %ix%i ] ITex: %x\n", tex->width, tex->height, tex->tex );
		else
			LogPrintf( PRN_ALL, "[ %s ] ITex: %x\n", tex->m_fileName.AsChar(), tex->tex );
		tex = tex->next;
	}
}

//---------------------------------------------------------
// CTexManager::CTexManager: 
//---------------------------------------------------------
CTexManager::CTexManager()
{
	m_currTex = 0;
	m_texList = 0;
}


//---------------------------------------------------------
// CTexManager::ReloadTexture: 
//---------------------------------------------------------
bool CTexManager::ReloadTexture(nsTexture *desc)
{
	if ( desc->bitmap )
	{
		if ( !ReloadBitmapTex( desc ) ) return false;
		//ri.printf( PRN_DEV, "after reload: %x\n", desc->tex );
		return true;
	}

	return ReloadFileTex( desc );
}

bool CTexManager::ReloadBitmapTex(nsTexture *desc)
{
	HRESULT	hr;
	IDirect3DSurface8	*surf = 0;
	IDirect3DSurface8	*dest = 0;

	try
	{
		hr = rs.m_pdev->CreateImageSurface( desc->width, desc->height, D3DFMT_A8R8G8B8, &surf );
		if ( FAILED(hr) ) throw "ReloadBitmapTex->CreateImageSurface";

		D3DLOCKED_RECT	lr;
		hr = surf->LockRect( &lr, 0, 0 );
		if ( FAILED(hr) ) throw "ReloadBitmapTex->LockRect";

		rgba_t	*c = (rgba_t*)desc->data;
		for ( int y = 0; y < desc->height; y++ )
		{
			uchar	*data = (uchar*)lr.pBits;
			fmt_argb_t	*line = (fmt_argb_t*)(data + (y * lr.Pitch));
			for ( int x = 0; x < desc->width; x++ )
			{
				int	i = y * desc->width + x;
				line[x].r = c[i].r;
				line[x].g = c[i].g;
				line[x].b = c[i].b;
				line[x].a = 255;//*/
				
				/*line[x].r = 255;
				line[x].g = 0;
				line[x].b = 0;
				line[x].a = 255;//*/
			}
		}

		hr = surf->UnlockRect();
		if ( FAILED(hr) ) throw "ReloadBitmapTex->UnlockRect";

		D3DFORMAT	fmt = desc->fmt == TF_RGB ? tm.m_texfmt_rgb : tm.m_texfmt;
		//D3DFORMAT	fmt = D3DFMT_UNKNOWN;

		desc->tex = 0;
		hr = rs.m_pdev->CreateTexture( desc->width, desc->height, desc->use_mipmap ? 0 : 1, 0, fmt, D3DPOOL_MANAGED, &desc->tex );
		if ( FAILED(hr) )
		{
			LogPrintf( PRN_ALL, "dev->CreateTexture - failed, call D3DXCreateTexture(...) - \n" );
			hr = D3DXCreateTexture( rs.m_pdev, desc->width, desc->height, desc->use_mipmap ? 0 : 1, 0, fmt, D3DPOOL_MANAGED, &desc->tex );
		}
		
		if ( FAILED(hr) )
		{
			throw "ReloadBitmapTex->CreateTexture";
			//throw "D3DXCreateTexture";
		}
		
		int	count = desc->tex->GetLevelCount();
		for ( int i = 0; i < count; i++ )
		{
			hr = desc->tex->GetSurfaceLevel( i, &dest );
			if ( FAILED(hr) ) throw "ReloadBitmapTex->GetSurfaceLevel";

			DWORD	filter = i ? D3DX_FILTER_BOX : D3DX_FILTER_NONE;

			hr = D3DXLoadSurfaceFromSurface( dest, 0, 0, surf, 0, 0, filter, 0 );
			if ( FAILED(hr) ) throw "ReloadBitmapTex->D3DXLoadSurfaceFromSurface";

			hr = dest->Release(); if ( FAILED(hr) )	throw "dest: ReloadBitmapTex->Release";
		}

		LogPrintf( PRN_DEV, "ITex: %x\n", desc->tex );
		hr = surf->Release(); if ( FAILED(hr) ) throw "surf: ReloadBitmapTex->Release";
	}
	catch ( char *error )
	{
		if ( FAILED(hr) )
			//d3d_FatalError( error, hr );
			LogPrintf( PRN_ALL, "%s: %s\n", error, d3d_GetError( hr ) );
		else
			LogPrintf( PRN_ALL, error );
			//ri.FatalError( error );
		desc->tex = 0;
		if ( surf ) surf->Release();
		if ( dest ) dest->Release();
	}
	
	return true;
}

bool CTexManager::ReloadFileTex(nsTexture *desc)
{
	nsFile	*file;
	HRESULT	hr;
	bool	notex = false;

	if ( !(file = g_pack.LoadFile( desc->m_fileName )) )
		return false;

	D3DPOOL	pool = D3DPOOL_MANAGED;

	D3DFORMAT	fmt = desc->fmt == TF_RGB ? tm.m_texfmt_rgb : tm.m_texfmt;
	
	hr = D3DXCreateTextureFromFileInMemoryEx( rs.m_pdev, file->GetData(), file->GetSize(),
		D3DX_DEFAULT, D3DX_DEFAULT, 
		desc->use_mipmap ? 0 : 1, 0, fmt, pool,
		D3DX_DEFAULT, D3DX_DEFAULT,
		0, 0, 0, &desc->tex );
	
	if ( FAILED(hr) )
	{
		LogPrintf( PRN_ALL, "D3DXCreateTextureFromFileInMemoryEx (): %s\n", d3d_GetError( hr ) );
		desc->tex = 0;
	}
	else	//get texture size
	{
		D3DSURFACE_DESC		surf_desc;
		memset( &surf_desc, 0, sizeof(surf_desc) );
		hr = desc->tex->GetLevelDesc( 0, &surf_desc );
		if ( FAILED(hr) )
			LogPrintf( PRN_ALL, "WARNING: can't retrieve texture size\n" );
		else
		{
			desc->width = surf_desc.Width;
			desc->height = surf_desc.Height;
		}
	}

	g_pack.ReleaseFile( file );
	return true;
}

//---------------------------------------------------------
// CTexManager::FindTexture: 
//---------------------------------------------------------
nsTexture* CTexManager::FindTexture( const char *filename, bool mipmap, texfmt_t fmt )
{
	if ( !filename ) return 0;
	
	nsTexture	*tex = tm.m_texList;
	while ( tex )
	{
		if ( tex->m_fileName == filename && 
			tex->use_mipmap == mipmap && 
			tex->fmt == fmt )
			return tex;
		tex = tex->next;
	}
	
	return 0;
}

//---------------------------------------------------------
// CTexManager::init: 
//---------------------------------------------------------
bool CTexManager::init()
{
	LogPrintf( PRN_ALL, "...init tm\n" );
	//texture format
	fmtdesc_t	*fmt;
	m_texfmt = d3d_GetTextureFormat( (int)r_tex_bits->Value(), rs.m_dev_type, rs.m_pp.BackBufferFormat, rs.m_pd3d );
	if ( !m_texfmt && r_tex_bits->Value() == 32 )
	{
		LogPrintf( PRN_ALL, "WARNING: 32 bit texture format unsupported, trying 16 bit!\n" );
		r_tex_bits->SetValue( 16 );
		m_texfmt = d3d_GetTextureFormat( (int)r_tex_bits->Value(), rs.m_dev_type, rs.m_pp.BackBufferFormat, rs.m_pd3d );
	}

	m_texfmt_rgb = d3d_GetTextureFormatRGB( (int)r_tex_bits->Value(), rs.m_dev_type, rs.m_pp.BackBufferFormat, rs.m_pd3d );

	if ( fmt = d3d_GetFormatDesc( m_texfmt ) )
		LogPrintf( PRN_ALL, "...using texture format for RGBA: %s (b:%i, c:%i)\n", fmt->d3desc, fmt->bits, fmt->components );
	if ( fmt = d3d_GetFormatDesc( m_texfmt_rgb ) )
		LogPrintf( PRN_ALL, "...using texture format for RGB: %s (b:%i, c:%i)\n", fmt->d3desc, fmt->bits, fmt->components );
			
	//texture filter
	rs.m_pdev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	rs.m_pdev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	rs.m_pdev->SetTextureStageState( 0, D3DTSS_MIPFILTER, r_tex_filter->Value() ? D3DTEXF_LINEAR : D3DTEXF_POINT );//*/

	LogPrintf( PRN_ALL, "...reload textures\n" );
	nsTexture	*tex = m_texList;
	while ( tex )
	{
		ReloadTexture( tex );
		tex = tex->next;
	}

	return true;
}

void CTexManager::release( bool bWithUserData )
{
	if ( bWithUserData )
	{
		LogPrintf( PRN_ALL, "...delete textures\n" );
		nsTexture	*tex = m_texList;
		while ( tex )
		{
			nsTexture	*del = tex;
			tex = tex->next;

			if ( del->bitmap )
			{
				LogPrintf( PRN_DEV, "[ %ix%i ] ITex: %x\n", del->width, del->height, del->tex );
				my_free( del->data );
			}
			else
				LogPrintf( PRN_DEV, "[ %s ] ITex: %x\n", del->m_fileName.AsChar(), del->tex );
			
			if ( del->tex )
			{
				HRESULT	hr = del->tex->Release();
				if ( FAILED(hr) )
					d3d_FatalError( "tex->Release", hr );
			}
			
			delete del;
		}

		m_texList = 0;//*/
	}
	else
	{
		LogPrintf( PRN_ALL, "...release texture vid mem\n" );
		nsTexture	*tex = m_texList;
		while ( tex )
		{
			if ( tex->tex )
			{
				tex->tex->Release();
				tex->tex = 0;
			}
			tex = tex->next;
		}
	}
}

//---------------------------------------------------------
// CTexManager::TextureLoad: 
//---------------------------------------------------------
nsTexture* CTexManager::TextureLoad( const char *fileName, bool mipmap, texfmt_t fmt )
{
	if ( !StrCheck( fileName ) ) return 0;

	//searching texture
	nsTexture	*ptex = tm.FindTexture( fileName, mipmap, fmt );
	if ( ptex )
	{
		//ri.printf( PRN_ALL, "found\n" );
		ptex->ref_count ++;
		return ptex;
	}

	//load if not found
	LogPrintf( PRN_ALL, "loading texture '%s' - ", fileName );
	nsTexture	*desc = new nsTexture;
	desc->id = RES_TEXTURE;
	desc->m_fileName = fileName;
	desc->use_mipmap = mipmap;
	desc->fmt = fmt;

	if ( tm.ReloadTexture( desc ) )
	{
		AddToList( &m_texList, desc );
		LogPrintf( PRN_ALL, "loaded\n" );
		return desc;
	}
	else
	{
		delete desc;
		LogPrintf( PRN_ALL, "fail\n" );
	}
	
	return 0;	

}

//---------------------------------------------------------
// CTexManager::TextureGenerate: 
//---------------------------------------------------------
nsTexture* CTexManager::TextureGenerate( int width, int height, const void *data, texfmt_t fmt, bool mipmap )
{
	LogPrintf( PRN_ALL, "generate texture (%i %i)\n", width, height );
	nsTexture	*desc = new nsTexture;
	
	desc->id = RES_TEXTURE;
	desc->use_mipmap = mipmap;
	desc->bitmap = true;
	desc->width = width;
	desc->height = height;
	desc->fmt = fmt;
	
	int	size = sizeof(uchar) * 4 * width * height;
	desc->data = (uchar*)my_malloc( size );
	memcpy( desc->data, data, size );

	if ( tm.ReloadTexture( desc ) )
	{
		AddToList( &m_texList, desc );
		LogPrintf( PRN_DEV, "after add tex: %x\n", desc->tex );
		return desc;
	}
	else
	{
		my_free( desc->data );
		delete desc;
	}

	return 0;
}

//---------------------------------------------------------
// CTexManager::TextureRelease: 
//---------------------------------------------------------
void CTexManager::TextureRelease( nsTexture *tex )
{
	if ( !tex ) return;

	if ( tex->id != RES_TEXTURE )
	{
		LogPrintf( PRN_DEV, "WARNING: release invalid texture!\n" );
		return;
	}

	if ( !tex->tex )
	{
		LogPrintf( PRN_DEV, "WARNING: release NULL texture object!\n" );
		return;
	}

	if ( !tex->ref_count )
	{
		if ( !tex->bitmap )
			LogPrintf( PRN_DEV, "delete texture: '%s'\n", tex->m_fileName.AsChar() );
		else
		{
			LogPrintf( PRN_DEV, "delete bitmap texture: %ix%i, ITex: %x\n", tex->width, tex->height, tex->tex  );
			my_free( tex->data );
		}

		tex->tex->Release();

		DelFromList( &m_texList, tex );
		delete tex;
	}
	else
		tex->ref_count --;//*/	
}

//---------------------------------------------------------
// CTexManager::TextureBind: 
//---------------------------------------------------------
void CTexManager::TextureBind( nsTexture *tex )
{
	if ( tex == m_currTex ) return;
	m_currTex = tex;
	
	if ( !tex )
	{
		rs.m_pdev->SetTexture( 0, 0 );
		return;
	}

	if ( tex->id != RES_TEXTURE || !tex->tex )
	{
		LogPrintf( PRN_DEV, "WARNING: invalid texture binding!!!\n" );
		return;
	}

	HRESULT	hr = rs.m_pdev->SetTexture( 0, tex->tex );
#ifdef _DEBUG
	if ( FAILED(hr) )
		LogPrintf( PRN_ALL, "WARNING: bind texture: %s\n", d3d_GetError( hr ) );
#endif	
}