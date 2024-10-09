#include "RenderState.h"
#include "resource.h"
#include "ConfigDlg.h"
#include "TexManager.h"
#include "States.h"
#include "VertexBufferManager.h"
#include "draw2d.h"

#include "nsLib/matrix4.h"
#include "nsLib/MathTools.h"
#include "nsLib/log.h"
#include "Core/sys.h"
#include "Core/Config.h"
#include "engine/RenManager.h"
#include "engine/ResourceMap.h"


//#define SYNC_RENDER

extern nsVar	*r_fov;
nsVar	*r_rawmesh_wire;
nsVar	*r_soft_texgen;
nsVar	*r_wireframe;
nsVar	*r_d3d_device;
nsVar	*r_tex_enable;

CRenderState	rs;

IRenDevice* GetRenDevice()
{
	return &rs;
}

void r_restart_f( int argc, const char *argv[] )
{
	rs.m_bRestart = true;
}

void d3d_FatalError( const char *str_func, HRESULT hr )
{
	Sys_FatalError( "D3D ERROR: %s: %s", str_func, d3d_GetError( hr ) );
}

//---------------------------------------------------------
// CRenderState::CRenderState: 
//---------------------------------------------------------
CRenderState::CRenderState()
{
	m_rsMgr = 0;
	m_pd3d = 0;
	m_pdev = 0;
	m_bDevLost = true;
	m_bFullScreen = false;
	m_bRestart = false;
	curr_tex = 0;
	//m_litAmb[0] = m_litAmb[1] = m_litAmb[2] = m_litAmb[3] = 0;
	memset( &m_currConfig, 0, sizeof(rasterConfig_t) );
	//m_lightEnabled = false;
}

//---------------------------------------------------------
// CRenderState::~CRenderState: 
//---------------------------------------------------------
CRenderState::~CRenderState()
{

}

//------------------------------------
// CRenderState::CheckCaps: 
//------------------------------------
void CRenderState::CheckCaps(UINT dev, D3DDEVTYPE type)
{
	LogPrintf( PRN_ALL, "...checking device caps\n" );
	D3DCAPS8	caps;
	memset( &caps, 0, sizeof(caps) );
	if ( !FAILED(m_pd3d->GetDeviceCaps( dev, type, &caps )) )
	{
		LogPrintf( PRN_ALL, "max texture size: %ix%i\n", caps.MaxTextureWidth, caps.MaxTextureHeight );
		
		caps_texgen = caps.VertexProcessingCaps & D3DVTXPCAPS_TEXGEN ? true : false;
		LogPrintf( PRN_ALL, "vertex texgen: %s\n", caps_texgen ? "supported" : "unsupported" );
		if ( r_soft_texgen->Value() && caps_texgen )
		{
			LogPrintf( PRN_ALL, "...using software texgen\n" );
			caps_texgen = false;
		}

		caps_managed = false;
		//caps_managed = caps.Caps2 & D3DCAPS2_CANMANAGERESOURCE ? true : false;
		//ri.printf( PRN_ALL, "manage resource: %s\n", caps_managed ? "yes" : "no" );
	}
}


//------------------------------------
// CRenderState::init: 
//------------------------------------
bool CRenderState::init()
{
	HRESULT	hr;
	m_dev_type = D3DDEVTYPE_HAL;
	//m_dev_type = D3DDEVTYPE_REF;
	LogPrintf( PRN_ALL, "initialize device\n" );

	if ( r_d3d_device->Value() < 0 || r_d3d_device->Value() >= m_pd3d->GetAdapterCount() )
		r_d3d_device->SetValue( 0 );

	D3DADAPTER_IDENTIFIER8	devid;
	memset( &devid, 0, sizeof(devid) );
	if ( !FAILED(m_pd3d->GetAdapterIdentifier( (UINT)r_d3d_device->Value(), 0, &devid )) )
		LogPrintf( PRN_ALL, "...using device: %s\n", devid.Description );

	CheckCaps( (UINT)r_d3d_device->Value(), m_dev_type );

	LogPrintf( PRN_ALL, "...enum modes\n" );
	if ( d3d_EnumModes( m_modes, (UINT)r_d3d_device->Value(), m_pd3d ) )
	{
		int index = m_modes.FindUserMode( (int)r_width->Value(), (int)r_height->Value() );
		if ( index < 0 ) 
		{
			LogPrintf( PRN_ALL, "WARNING: specified mode not found!\n" );
			index = 0;
		}

		mode_desc_t	*m = m_modes.GetMode( index );
		r_width->SetValue( (float)m->width );
		r_height->SetValue( (float)m->height );
		r_refresh->SetValue( (float)m->refresh );

		m_modes.Clear();
	}
	else
		LogPrintf( PRN_ALL, "WARNING: no display modes found\n" );

	D3DDISPLAYMODE d3ddm;
	memset( &d3ddm, 0, sizeof(d3ddm) );
	hr = m_pd3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );
	if ( FAILED(hr) )
	{
		d3d_FatalError( "GetAdapterDisplayMode", hr );
		return false;
	}
	UpdateCurrConfig( d3ddm );

	memset( &m_pp, 0, sizeof(m_pp) );
	m_pp.hDeviceWindow = m_hWnd;
	m_pp.EnableAutoDepthStencil = TRUE;
	m_pp.AutoDepthStencilFormat = D3DFMT_D16;
#ifdef SYNC_RENDER
	m_pp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
#endif

reset:
	AdjustWindow( m_hWnd, (int)r_width->Value(), (int)r_height->Value(), r_fullscreen->Value() != 0 );

	if ( r_fullscreen->Value() )
	{
		LogPrintf( PRN_ALL, "...init fullscreen mode: %ix%ix%i\n", (int)r_width->Value(), (int)r_height->Value(), (int)r_pixel_bits->Value() );
		D3DFORMAT	fmt = d3d_BackBufferFormat( (UINT)r_d3d_device->Value(), m_dev_type, (int)r_pixel_bits->Value(), m_pd3d );
		if ( !fmt && r_pixel_bits->Value() == 32 )
		{
			LogPrintf( PRN_ALL, "WARNING: 32 bit color depth unsupported, trying 16 bit!\n" );
			r_pixel_bits->SetValue( 16 );
			fmt = d3d_BackBufferFormat( (UINT)r_d3d_device->Value(), m_dev_type, (int)r_pixel_bits->Value(), m_pd3d );
		}

		fmtdesc_t	*f = d3d_GetFormatDesc( fmt );
		if ( f )
			LogPrintf( PRN_ALL, "...using backbuffer format: %s (b:%i,c:%i)\n", f->d3desc, f->bits, f->components );

		m_pp.BackBufferWidth = (UINT)r_width->Value();
		m_pp.BackBufferHeight = (UINT)r_height->Value();
		m_pp.BackBufferFormat = fmt;
		m_pp.FullScreen_RefreshRateInHz = (UINT)r_refresh->Value();
		m_pp.FullScreen_PresentationInterval = r_vsync->Value() ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
		m_pp.SwapEffect = D3DSWAPEFFECT_DISCARD;

		m_bFullScreen = true;
	}
	else
	{
		LogPrintf( PRN_ALL, "...init windowed mode: %ix%i\n", (int)r_width->Value(), (int)r_height->Value() );


		d3ddm.Format = d3d_WinBackBufferFormat( (UINT)r_d3d_device->Value(), m_dev_type, d3ddm.Format, m_pd3d );

		if ( !d3ddm.Format )
		{
			LogPrintf( PRN_ALL, "WARNING: desktop mode unsupport hardware acceleration!\n" );
			r_fullscreen->SetValue( 1 );
			goto reset;
		}
		
		fmtdesc_t	*f = d3d_GetFormatDesc( d3ddm.Format );
		if ( f )
		{
			LogPrintf( PRN_ALL, "...using backbuffer format: %s (b:%i,c:%i)\n", f->d3desc, f->bits, f->components );
			r_pixel_bits->SetValue( (float)f->bits );
		}

		m_pp.Windowed   = TRUE;
		m_pp.SwapEffect = r_vsync->Value() ? D3DSWAPEFFECT_COPY_VSYNC : D3DSWAPEFFECT_FLIP;
		m_pp.BackBufferCount = 1;
		m_pp.BackBufferFormat = d3ddm.Format; 

		m_bFullScreen = false;
	}

	m_currConfig.currWidth = (uint)r_width->Value();
	m_currConfig.currHeight = (uint)r_height->Value();
	
	LogPrintf( PRN_ALL, "...create device\n" );
	hr = m_pd3d->CreateDevice( (UINT)r_d3d_device->Value(), m_dev_type, m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_pp, &m_pdev );
	if ( FAILED(hr) )
	{
		hr = m_pd3d->CreateDevice( (UINT)r_d3d_device->Value(), m_dev_type, m_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_pp, &m_pdev );
		if ( FAILED(hr) )
		{
			d3d_FatalError( "CreateDevice", hr );
			return false;
		}
		LogPrintf( PRN_ALL, "...using software vertex processing\n" );
	}
	else
		LogPrintf( PRN_ALL, "...using hardware vertex processing\n" );
	
	//view depth stencil format
	IDirect3DSurface8	*surf;
	hr = m_pdev->GetDepthStencilSurface( &surf );
	if ( !FAILED(hr) )
	{
		D3DSURFACE_DESC	desc;
		memset( &desc, 0, sizeof(desc) );
		hr = surf->GetDesc( &desc );
		if ( !FAILED(hr) )
		{
			fmtdesc_t *fmt = d3d_GetFormatDesc( desc.Format );
			if ( fmt )
				LogPrintf( PRN_ALL, "...using depth stencil format: %s (b:%i)\n", fmt->d3desc, fmt->bits );
		}
		surf->Release();
	}
	else
		LogPrintf( PRN_ALL, "WARNING: can't get depth/stencil surface!\n" );

	if ( !ReloadResources() ) return false;
	SetBrightness( r_brightness->Value() );
	m_bDevLost = false;
	return true;
}

/*--------------------------------------------
--------------------------------------------*/
void CRenderState::release(bool bUserData)
{
	sb.release();
	tm.release( bUserData );
	g_vbManager.Release( bUserData );

	if ( m_pdev )
	{
		LogPrintf( PRN_ALL, "...release device\n" );
		m_pdev->Release();
		m_pdev = 0;
	}
}

/*--------------------------------------------
--------------------------------------------*/
bool CRenderState::ResetDevice()
{
	if ( !m_bDevLost ) return true;
	HRESULT	hr;

	D3DDISPLAYMODE d3ddm;
	memset( &d3ddm, 0, sizeof(d3ddm) );
	hr = m_pd3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );
	if ( FAILED(hr) )
	{
		d3d_FatalError( "GetAdapterDisplayMode", hr );
		return false;
	}
	UpdateCurrConfig( d3ddm );

	InvalidateResources();

	LogPrintf( PRN_ALL, "=== restoring device ===\n" );
	if ( !m_bFullScreen ) m_pp.BackBufferFormat = d3ddm.Format;
		
	hr = m_pdev->Reset( &m_pp );
	if ( hr != D3D_OK )
	{
		LogPrintf( PRN_ALL, "failed to reset: %s\n", d3d_GetError( hr ) );
		return false;
	}
	else
	{
		//AdjustWindow( m_hWnd, (int)r_width->value, (int)r_height->value, r_fullscreen->value != 0 );
		if ( !ReloadResources() ) return false;
		LogPrintf( PRN_ALL, "=== device restored successfull ===\n" );
		m_bDevLost = false;
	}

	return true;
}

//---------------------------------------------------------
// CRenderState::ReloadResources:
//---------------------------------------------------------
bool CRenderState::ReloadResources()
{
	if ( !g_vbManager.Init( m_pdev ) ) return false;
	if ( !sb.init( m_pdev ) ) return false;
	if ( !tm.init() ) return false;

	D3DXMatrixIdentity( &m_mIdent );

	D3DMATERIAL8 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	m_pdev->SetMaterial( &mtrl );

	//imp_SetLightAmbient( m_litAmb );
	m_pdev->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	m_pdev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	m_pdev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	m_pdev->SetRenderState( D3DRS_LIGHTING, FALSE );

		
	m_pdev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	m_pdev->SetRenderState( D3DRS_LIGHTING, FALSE );

	m_pdev->LightEnable( 0, FALSE );
	m_pdev->LightEnable( 1, FALSE );
	m_pdev->LightEnable( 2, FALSE );
	m_pdev->LightEnable( 3, FALSE );
	return true;
}

void CRenderState::InvalidateResources()
{
	tm.release( false );
	sb.release();
	g_vbManager.Release( false );
}

//---------------------------------------------------------
// CRenderState::SyncRender: 
//---------------------------------------------------------
void CRenderState::SyncRender()
{
	HRESULT	hr;
	IDirect3DSurface8	*backBuff;
	hr = m_pdev->GetRenderTarget( &backBuff );
	if ( FAILED(hr) )
	{
		LogPrintf( PRN_ALL, "WARNING: get back buff: %s\n", d3d_GetError( hr ) );
		return;
	}

	RECT	rect = { 0, 0, 1, 1 };
	D3DLOCKED_RECT	data;
	hr = backBuff->LockRect( &data, &rect, D3DLOCK_READONLY );
	if ( FAILED(hr) )
	{
		LogPrintf( PRN_ALL, "WARNING: lock back buff: %s\n", d3d_GetError( hr ) );
		backBuff->Release();
		return;
	}

	byte tmp = *((byte*)data.pBits);

	hr = backBuff->UnlockRect();
	if ( FAILED(hr) )
		LogPrintf( PRN_ALL, "WARNING: unlock back buff: %s\n", d3d_GetError( hr ) );//*/

	hr = backBuff->Release();
	if ( FAILED(hr) )
		LogPrintf( PRN_ALL, "WARNING: release back buff: %s\n", d3d_GetError( hr ) );
}

//---------------------------------------------------------
// CRenderState::SetBrightness: 
//---------------------------------------------------------
void CRenderState::SetBrightness( float value )
{
	if ( value < 0 ) value = 0;
	else if ( value > 1 ) value = 1;
	
	SetRamp( value * 2.0f, 1, 1 );
	m_prev_bright = value;
	r_brightness->SetValue( value );

	LogPrintf( PRN_ALL, "set brightness: %f\n", value );
}

void CRenderState::SetRamp( float gamma, float contrast, float brightness )
{
	D3DGAMMARAMP ramp;
	float angle = contrast;
	float offset = (brightness-1)*256;
	for (int i = 0; i < 256; i++)
	{
		float k = i / 256.0f;
		k = (float)pow( k, 1.0f / gamma );
		k = k*256;
		float value = k*angle*256+offset*256;
		if (value > 65535) value = 65535;
		if (value < 0) value = 0;
		
		ramp.red[i] = (uword)value;
		ramp.green[i] = (uword)value;
		ramp.blue[i] = (uword)value;
	}
	m_pdev->SetGammaRamp( 0, &ramp );
}


//------------------------------------
// CRenderState::AdjustWindow: 
//------------------------------------
void CRenderState::AdjustWindow(HWND hWnd, int width, int height, bool fullscreen)
{
	RECT	rect;
	rect.left = rect.top = 0;
	rect.right = width;
	rect.bottom = height;

	AdjustWindowRect( &rect, GetWindowLong( hWnd, GWL_STYLE ), FALSE );
	
	SetWindowPos( hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );	//style apply
	SetWindowPos( hWnd, fullscreen ? HWND_TOPMOST : HWND_NOTOPMOST, 
		0, 0, rect.right - rect.left, rect.bottom - rect.top, 
		SWP_NOCOPYBITS | SWP_SHOWWINDOW );	//position and z order apply//*/

	GetClientRect( hWnd, &rect );
	LogPrintf( PRN_DEV, "...client size: %ix%i\n", rect.right, rect.bottom );
}

void CRenderState::UpdateCurrConfig(D3DDISPLAYMODE &d3dmode)
{
	fmtdesc_t	*fmt = d3d_GetFormatDesc( d3dmode.Format );
	if ( fmt ) m_currConfig.defPixelBits = fmt->bits;
	m_currConfig.defScreenWidth = d3dmode.Width;
	m_currConfig.defScreenHeight = d3dmode.Height;
}

//---------------------------------------------------------
// CRenderState::GetName: 
//---------------------------------------------------------
const char* CRenderState::GetName()
{
	return "Direct3D 8";	
}

//---------------------------------------------------------
// CRenderState::ConfigDlg: 
//---------------------------------------------------------
bool CRenderState::ConfigDlg( uint hWnd, const char *localFilePath, const char *localPostfix )
{
	dlgCfg.SetLocalDesc( localFilePath, localPostfix );
	int err = dlgCfg.DoModal( GetModuleHandle( 0 ), (HWND)hWnd, MAKEINTRESOURCE( IDD_CONFIG_DIALOG ) );
	return true;
}

//---------------------------------------------------------
// CRenderState::Init: 
//---------------------------------------------------------
bool CRenderState::Init( void *hWnd )
{
	if ( !hWnd || !IsWindow( (HWND)hWnd ) )
	{
		Sys_FatalError( "D3D need a window handle identifier" );
		return false;
	}

	LogPrintf( PRN_ALL,
		"------------------------------------\n"\
		" initialize Direct3D Render         \n"\
		"------------------------------------\n" );

	LogPrintf( PRN_ALL, "...create d3d\n" );
	if ( !(m_pd3d = Direct3DCreate8( D3D_SDK_VERSION )) )
	{
		Sys_FatalError( "Can't create Direct3D8\n" );
		return false;
	}

	r_d3d_device = g_cfg->RegVar( "r_d3d_device", "0", GVF_SAVABLE | GVF_READONLY );	

	g_cfg->RegCmd( "tex_reload", CTexManager::tm_reload_texture_f );
	g_cfg->RegCmd( "tex_reload_all", CTexManager::tm_reload_textures_f );
	g_cfg->RegCmd( "tex_list", CTexManager::tm_list_f );

	g_cfg->RegCmd( "r_restart", r_restart_f );

	r_fov = g_cfg->RegVar( "r_fov", "60", 0 );
	r_rawmesh_wire = g_cfg->RegVar( "r_rawmesh_wire", "0", 0 );
	r_soft_texgen = g_cfg->RegVar( "r_soft_texgen", "0", 0 );
	r_wireframe = g_cfg->RegVar( "r_wireframe", "0", 0 );
	r_tex_enable = g_cfg->RegVar( "r_tex_enable", "1", 0 );

	m_hWnd = (HWND)hWnd;
	m_rsMgr = new nsRenStateMgr;
	return init();
}

//---------------------------------------------------------
// CRenderState::Release: 
//---------------------------------------------------------
void CRenderState::Release()
{
	LogPrintf( PRN_ALL,
		"------------------------------------\n"\
		" Shuttingdown Direct3D Render       \n"\
		"------------------------------------\n" );
	delete m_rsMgr;
	release( true );
	if ( m_pd3d )
	{
		LogPrintf( PRN_ALL, "...release d3d\n" );
		COM_RELEASE( m_pd3d );
	}	
}

//---------------------------------------------------------
// CRenderState::GetModeList: 
//---------------------------------------------------------
int CRenderState::GetModeList( int **modes )
{
	*modes = 0;
	int	count = 0;
	
	CModeArray	m;
	CEasyArray<int>	ma;
	if ( !d3d_EnumModes( m, (UINT)r_d3d_device->Value(), m_pd3d ) )
		return 0;

	for ( int i = 0; i < m.count; i++ )
	{
		mode_desc_t	*md = m.GetMode( i );
		if ( md )
		{
			ma.add_data( &*modes, count, md->width );
			ma.add_data( &*modes, count, md->height );
		}
	}

	m.Clear();
	return count / 2;	
}

//---------------------------------------------------------
// CRenderState::GetCurrentConfig: 
//---------------------------------------------------------
const rasterConfig_t* CRenderState::GetCurrentConfig()
{
	return &m_currConfig;
}

//---------------------------------------------------------
// CRenderState::SetColor: 
//---------------------------------------------------------
void CRenderState::SetColor( const float c[4] )
{
	m_color = D3DCOLOR_RGBA( int(c[0] * 255.0), int(c[1] * 255.0), int(c[2] * 255.0), int(c[3] * 255.0) );	
}

//---------------------------------------------------------
// CRenderState::TextureLoad: 
//---------------------------------------------------------
ITexture* CRenderState::TextureLoad( const char *filename, bool mipmap, texfmt_t fmt )
{
	if ( !StrCheck( filename ) ) return 0;

	nsString	name = g_resMap.TextureFile( filename );
	return tm.TextureLoad( name, mipmap, fmt );
}

//---------------------------------------------------------
// CRenderState::TextureGenerate: 
//---------------------------------------------------------
ITexture* CRenderState::TextureGenerate( int width, int height, const void *data, texfmt_t fmt, bool mipmap )
{
	return tm.TextureGenerate( width, height, data, fmt, mipmap );
}

//---------------------------------------------------------
// CRenderState::TextureRelease: 
//---------------------------------------------------------
void CRenderState::TextureRelease( ITexture *texture )
{
	tm.TextureRelease( (nsTexture*)texture );
}

//---------------------------------------------------------
// CRenderState::TextureBind: 
//---------------------------------------------------------
void CRenderState::TextureBind( ITexture *texture )
{
	if ( r_tex_enable->Value() )
		tm.TextureBind( (nsTexture*)texture );
	else
		tm.TextureBind( 0 );
}

//---------------------------------------------------------
// CRenderState::TextureTranform: 
//---------------------------------------------------------
void CRenderState::TextureTranform( const float *offs2, const float *scale2 )
{
	nsMatrix	m;
	m.Identity();

	if ( offs2 )
	{
		m._31 = offs2[0];
		m._32 = offs2[1];
	}
	if ( scale2 )
	{
		m._11 = scale2[0];
		m._22 = scale2[1];
	}

	m_pdev->SetTransform( D3DTS_TEXTURE0, (D3DMATRIX*)&m );	
}

//---------------------------------------------------------
// CRenderState::StateLoad: 
//---------------------------------------------------------
IRenState* CRenderState::StateLoad( const char *fileName )
{
	return m_rsMgr->LoadState( fileName );
}

//---------------------------------------------------------
// CRenderState::StateRelease: 
//---------------------------------------------------------
void CRenderState::StateRelease( IRenState *state )
{
	nsRenState	*s = (nsRenState*)state;
	m_rsMgr->ReleaseState( s );
}

//---------------------------------------------------------
// CRenderState::StateApply: 
//---------------------------------------------------------
void CRenderState::StateApply( IRenState *state )
{
	m_rsMgr->ApplyState( m_pdev, (nsRenState*)state );
}

//---------------------------------------------------------
// CRenderState::ClearScene: 
//---------------------------------------------------------
void CRenderState::ClearScene( uint flags )
{
	DWORD	f = 0;
	
	if ( flags & CLR_ZBUFF )
		f |= D3DCLEAR_ZBUFFER;
	
	if ( flags & CLR_CBUFF )
		f |= D3DCLEAR_TARGET;

	HRESULT	hr;
	hr = m_pdev->Clear( 0, NULL,  f, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
	if ( FAILED(hr) )
		d3d_FatalError( "Clear", hr );	
}

bool CRenderState::BeginScene()
{
	if ( !m_pdev ) return false;
	
	if ( m_bRestart )
	{
		//ShowWindow( m_hWnd, SW_HIDE );
		InvalidateRect( m_hWnd, 0, TRUE );
		release( false );
		init();
		m_bRestart = false;
		//ShowWindow( m_hWnd, SW_SHOW );
	}

	IDirect3DDevice8	*dev = m_pdev;
	HRESULT	hr;

	hr = dev->TestCooperativeLevel();
	if ( hr == D3DERR_DEVICELOST  )
	{
		m_bDevLost = true;
		return false;
	}
	else if ( hr == D3DERR_DEVICENOTRESET )
	{
		m_bDevLost = true;
		if ( !ResetDevice() ) return false;
	}

#ifdef SYNC_RENDER
	SyncRender();
#endif
	
	hr = dev->BeginScene();
	if ( FAILED(hr) )
	{
		d3d_FatalError( "BeginScene", hr );
	}

	if ( r_wireframe->Value() )
		m_pdev->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	else
		m_pdev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

	return true;
}

//---------------------------------------------------------
// CRenderState::EndScene: 
//---------------------------------------------------------
void CRenderState::EndScene()
{
	HRESULT	hr;
	if ( !m_bDevLost )
	{
		hr = m_pdev->EndScene();
		if ( FAILED(hr) )
		{
			d3d_FatalError( "EndScene", hr );
			return;
		}
	
		hr = m_pdev->Present( 0, 0, 0, 0 );
		if ( hr != D3DERR_DEVICELOST )
		{
			if ( FAILED(hr) )
			{
				d3d_FatalError( "Present", hr );
				return;
			}
		}
		else
			return;

		if ( m_prev_bright != r_brightness->Value() )
		{
			SetBrightness( r_brightness->Value() );
			LogPrintf( PRN_ALL, "bright\n" );
		}
	}	
}

//---------------------------------------------------------
// CRenderState::LoadProjMatrix: 
//---------------------------------------------------------
void CRenderState::LoadProjMatrix( const float m[16] )
{
	m_pdev->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)m );
}

//---------------------------------------------------------
// CRenderState::LoadViewMartix: 
//---------------------------------------------------------
void CRenderState::LoadViewMartix( const float m[16] )
{
	m_pdev->SetTransform( D3DTS_VIEW, (D3DMATRIX*)m );
}

//---------------------------------------------------------
// CRenderState::LoadMatrix: 
//---------------------------------------------------------
void CRenderState::LoadMatrix( const float m[16] )
{
	m_pdev->SetTransform( D3DTS_WORLD, (D3DMATRIX*)m );	
}

void CRenderState::MultMatrix( const float m[16] )
{
	nsMatrix	tmp;
	m_pdev->GetTransform( D3DTS_WORLD, (D3DMATRIX*)&tmp );
	m_pdev->SetTransform( D3DTS_WORLD, (D3DMATRIX*)m );
	m_pdev->MultiplyTransform( D3DTS_WORLD, (D3DMATRIX*)&tmp );
	
}

void CRenderState::MultMatrixLocal( const float m[16] )
{
	m_pdev->MultiplyTransform( D3DTS_WORLD, (D3DMATRIX*)m );
}

typedef struct
{
	float	x, y, z;
	DWORD	c;
	float	tu, tv;
}
ch_vert_t;

#define FVF_CH_VERT	(D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE)

//---------------------------------------------------------
// CRenderState::DrawSprite: 
//---------------------------------------------------------
void CRenderState::DrawSprite( float x, float y, float w, float h, float tu1 /*= 0*/, float tv1 /*= 0*/, float tu2 /*= 1*/, float tv2 /*= 1 */ )
{
	ch_vert_t	spr[4] = 
	{
		{ x,		y, 0, m_color, tu1, tv1 },
		{ x,		y - h, 0, m_color, tu1, tv2 },
		{ w + x,	y - h, 0, m_color, tu2, tv2 },
		{ w + x,	y, 0, m_color, tu2, tv1 },
		
	};
	HRESULT	hr;

	m_pdev->SetVertexShader( FVF_CH_VERT );
	hr = m_pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, spr, sizeof(ch_vert_t) );

#ifdef _DEBUG
	if ( FAILED(hr) )
		LogPrintf( PRN_ALL, "ERROR: draw sprite\n" );
#endif
}

//---------------------------------------------------------
// CRenderState::DrawLinedSprite: 
//---------------------------------------------------------
void CRenderState::DrawLinedSprite( float x1, float y1, float x2, float y2, float width, float height )
{
	nsVec2	p1( x1, y1 );
	nsVec2	dir = (nsVec2( x2, y2 ) - p1) * 0.5f;
	p1 += dir;
	dir.Norm();

	nsVec2	down = dir.GetRight();

	p1 -= dir * width * 0.5f;
	p1 -= down * height * 0.5f;

	dir = dir * width;
	down = down * height;


	ch_vert_t	spr[4] = 
	{
		{ 0, 0, 0, m_color, 0, 0 },
		{ 0, 0, 0, m_color, 0, 1 },
		{ 0, 0, 0, m_color, 1, 1 },
		{ 0, 0, 0, m_color, 1, 0 },
	};//*/

	spr[0].x = p1.x;
	spr[0].y = p1.y;

	nsVec2	tmp = p1 + down;
	spr[1].x = tmp.x;
	spr[1].y = tmp.y;

	tmp = tmp + dir;
	spr[2].x = tmp.x;
	spr[2].y = tmp.y;

	tmp = p1 + dir;
	spr[3].x = tmp.x;
	spr[3].y = tmp.y;
	
	HRESULT	hr;
	m_pdev->SetTransform( D3DTS_WORLD, &m_mIdent );
	m_pdev->SetVertexShader( FVF_CH_VERT );
	hr = m_pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, spr, sizeof(ch_vert_t) );

#ifdef _DEBUG
	if ( FAILED(hr) )
		LogPrintf( PRN_ALL, "ERROR: draw sprite\n" );
#endif	
}

void CRenderState::DrawFadeScreen()
{
	ch_vert_t	v[4] = 
	{
		{ 0,	0,	 0, m_color, 0, 0 },
		{ 640,	0,	 0, m_color, 1, 0 },
		{ 640,	480, 0, m_color, 1, 1 },
		{ 0,	480, 0, m_color, 0, 1 }
	};

	nsMatrix	m;
	m.Identity();
	m_pdev->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&m );
	
	m_pdev->SetVertexShader( FVF_CH_VERT );
	HRESULT	hr = m_pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, v, sizeof(ch_vert_t) );

#ifdef _DEBUG
	if ( FAILED(hr) )
		LogPrintf( PRN_DEV, "ERROR: draw char\n" );
#endif	
}

void CRenderState::DrawChar( float x, float y, rchar_t *ch )
{
	if ( !ch ) return;
	
	ch_vert_t	v[4] = 
	{
		{ 0,			0,			 0, m_color, ch->coord[0],					ch->coord[1] + ch->tex_size[1] },
		{ ch->size[0],	0,			 0, m_color, ch->coord[0] + ch->tex_size[0], ch->coord[1] + ch->tex_size[1] },
		{ ch->size[0],	ch->size[1], 0, m_color, ch->coord[0] + ch->tex_size[0], ch->coord[1] },
		{ 0,			ch->size[1], 0, m_color, ch->coord[0], ch->coord[1] }//*/
	};

	nsMatrix	m;
	m.Identity();
	
	m._41 = x + ch->offs[0];
	m._42 = y + ch->offs[1];
	m_pdev->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&m );
	
	m_pdev->SetVertexShader( FVF_CH_VERT );
	HRESULT	hr = m_pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, v, sizeof(ch_vert_t) );

#ifdef _DEBUG
	if ( FAILED(hr) )
		LogPrintf( PRN_DEV, "ERROR: draw char\n" );
#endif	
}

void CRenderState::DrawCharScaled( float x, float y, rchar_t *ch, float sx, float sy )
{
	if ( !ch ) return;

	float	size_x = ch->size[0] * sx;
	float	size_y = ch->size[1] * sy;
	float	posX = x + ch->offs[0] * sx;
	float	posY = y + ch->offs[1] * sy;
	
	ch_vert_t	v[4] = 
	{
		{ posX,				posY,		0, m_color, ch->coord[0],					ch->coord[1] + ch->tex_size[1] },
		{ size_x + posX,	posY,		0, m_color, ch->coord[0] + ch->tex_size[0], ch->coord[1] + ch->tex_size[1] },
		{ size_x + posX,	size_y + posY, 0, m_color, ch->coord[0] + ch->tex_size[0], ch->coord[1] },
		{ posX,				size_y + posY, 0, m_color, ch->coord[0], ch->coord[1] }//*/
	};

	m_pdev->SetVertexShader( FVF_CH_VERT );
	HRESULT	hr = m_pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, v, sizeof(ch_vert_t) );

#ifdef _DEBUG
	if ( FAILED(hr) )
		LogPrintf( PRN_DEV, "ERROR: draw char scaled\n" );
#endif//*/	
}

//---------------------------------------------------------
// CRenderState::VerticesCreate: 
//---------------------------------------------------------
IVertexBuffer* CRenderState::VerticesCreate( uint vertsCount, uint indexCount, bool dynamic /*= false */ )
{
	return g_vbManager.CreateVertexBuffer( vertsCount, indexCount, dynamic );
}

//---------------------------------------------------------
// CRenderState::VerticesRelease: 
//---------------------------------------------------------
void CRenderState::VerticesRelease( IVertexBuffer* vb )
{
	g_vbManager.ReleaseVertexBuffer( (nsVertexBuffer*)vb );	
}

//---------------------------------------------------------
// CRenderState::VerticesDraw: 
//---------------------------------------------------------
void CRenderState::VerticesDraw( IVertexBuffer* vb )
{
	if ( !vb ) return;
	nsVertexBuffer	*buf = (nsVertexBuffer*)vb;
	buf->Draw( g_vbManager.m_dev );
}


//---------------------------------------------------------
// CRenderState::DrawLine: 
//---------------------------------------------------------
typedef struct
{
	float	x, y, z;
	DWORD	color;
}
line_vert_t;

#define	FVF_LINE_VERT	(D3DFVF_XYZ | D3DFVF_DIFFUSE)

void CRenderState::DrawLine( float x1, float y1, float z1, float x2, float y2, float z2 )
{
	line_vert_t	v[2] = 
	{
		{ x1, y1, z1, m_color },
		{ x2, y2, z2, m_color },
	};
	
	m_pdev->SetVertexShader( FVF_LINE_VERT );
	HRESULT	hr = m_pdev->DrawPrimitiveUP( D3DPT_LINELIST, 1, v, sizeof(line_vert_t) );

#ifdef _DEBUG
	if ( FAILED(hr) )
		LogPrintf( PRN_ALL, "DrawLine() error: %s\n", d3d_GetError( hr ) );
#endif	
}

//---------------------------------------------------------
// CRenderState::DrawSprite3D: 
//---------------------------------------------------------
void CRenderState::DrawSprite3D( const nsMatrix &view, const nsVec3 &pos, float width, float height, float angle, float w_scale /*= 1 */ )
{
	nsVec3	up, right;
	up.x = view._12;
	up.y = view._22;
	up.z = view._32;
	
	right.x = view._11;
	right.y = view._21;
	right.z = view._31; 

	if ( angle )
	{
		nsMatrix	m;
		m.AxisRotate( up.Cross( right ), angle );
		up *= m;
		right *= m;
	}//*/

	float	w = width / 2.0f;
	float	h = height / 2.0f;

	up *= h;
	right *= w;

	color_vert_t	v[4];
	v[0].tu = 0;	v[0].tv = 0;
	v[1].tu = 0;	v[1].tv = 1;
	v[2].tu = w_scale;	v[2].tv = 1;
	v[3].tu = w_scale;	v[3].tv = 0;
	
	v[0].c = v[1].c = v[2].c = v[3].c = m_color;

	v[0].v = pos + (up - right);
	v[1].v = pos - (up + right);
	if ( w_scale != 1 )
	{
		right *= 2.0f * w_scale;
		v[3].v = v[0].v + right;
		v[2].v = v[1].v + right;
	}
	else
	{
		v[2].v = pos - (up - right);
		v[3].v = pos + (up + right);
	}

	m_pdev->SetVertexShader( FVF_COLOR_VERT );
	/*if ( angle )
	{
		D3DXMATRIX	mat;
		D3DXMatrixRotationAxis( &mat, (D3DXVECTOR3*)&vec3cross( up, right ), angle );
		m_pdev->SetTransform( D3DTS_WORLD, &mat );
	}
	else//*/
	m_pdev->SetTransform( D3DTS_WORLD, &m_mIdent );
	HRESULT	hr = m_pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, v, sizeof(color_vert_t) );

#ifdef _DEBUG
	if ( FAILED(hr) )
		LogPrintf( PRN_ALL, "DrawSprite3D() error: %s\n", d3d_GetError( hr ) );
#endif	
}

//---------------------------------------------------------
// CRenderState::DrawChar3D: 
//---------------------------------------------------------
void CRenderState::DrawChar3D( rchar_t *ch, nsVec3 &pos, float sx, float sy )
{
	if ( !ch ) return;

	float	w = ch->size[0] * sx;
	float	h = ch->size[1] * sy;

	color_vert_t	v[4];
	v[0].tu = ch->coord[0];						v[0].tv = ch->coord[1] + ch->tex_size[1];
	v[1].tu = ch->coord[0] + ch->tex_size[0];	v[1].tv = ch->coord[1] + ch->tex_size[1];
	v[2].tu = ch->coord[0] + ch->tex_size[0];	v[2].tv = ch->coord[1];
	v[3].tu = ch->coord[0];						v[3].tv = ch->coord[1];

	v[0].v.x = pos.x;		v[0].v.y = pos.y;		v[0].v.z = pos.z;
	v[1].v.x = pos.x + w;	v[1].v.y = pos.y;		v[1].v.z = pos.z;
	v[2].v.x = pos.x + w;	v[2].v.y = pos.y + h;	v[2].v.z = pos.z;
	v[3].v.x = pos.x;		v[3].v.y = pos.y + h;	v[3].v.z = pos.z;

	v[0].c = v[1].c = v[2].c = v[3].c = m_color;

	m_pdev->SetVertexShader( FVF_COLOR_VERT );
	m_pdev->SetTransform( D3DTS_WORLD, &m_mIdent );
	HRESULT	hr = m_pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, v, sizeof(color_vert_t) );

#ifdef _DEBUG
	if ( FAILED(hr) )
		LogPrintf( PRN_ALL, "DrawChar3D() error: %s\n", d3d_GetError( hr ) );
#endif	
}

//---------------------------------------------------------
// CRenderState::DrawPlaneSprite: 
//---------------------------------------------------------
void CRenderState::DrawPlaneSprite( const nsMatrix &view, const nsVec3& p1, const nsVec3& p2, float width, uint wrap_count )
{
	nsVec3	l, w, dir;
	
	dir.x = -view._13;
	dir.y = -view._23;
	dir.z = -view._33; 

	l = p2 - p1;

	w = dir.Cross( l );
	w.Norm();
	w *= width / 2.0f;

	color_vert_t	v[4];
	v[0].tu = 0;	v[0].tv = 0;
	v[1].tu = 0;	v[1].tv = (float)wrap_count;
	v[2].tu = 1;	v[2].tv = (float)wrap_count;
	v[3].tu = 1;	v[3].tv = 0;
	
	v[0].c = v[1].c = v[2].c = v[3].c = m_color;

	v[0].v = p2 - w;
	v[1].v = p1 - w;
	v[2].v = p1 + w;
	v[3].v = p2 + w;

	m_pdev->SetVertexShader( FVF_COLOR_VERT );
	m_pdev->SetTransform( D3DTS_WORLD, &m_mIdent );

	HRESULT	hr = m_pdev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, v, sizeof(color_vert_t) );

#ifdef _DEBUG
	if ( FAILED(hr) )
		LogPrintf( PRN_ALL, "imp_DrawPlaneSprite() error: %s\n", d3d_GetError( hr ) );
#endif	
}

//---------------------------------------------------------
// CRenderState::Lighting: 
//---------------------------------------------------------
void CRenderState::Lighting( bool enable )
{
	m_pdev->SetRenderState( D3DRS_LIGHTING, enable ? TRUE : FALSE );
}

//---------------------------------------------------------
// CRenderState::LightEnable: 
//---------------------------------------------------------
void CRenderState::LightEnable( int source, const nsVec3 &dir, const float c[4] )
{
	//if ( !m_lightEnabled ) return;

	D3DLIGHT8	lit;
	memset( &lit, 0, sizeof(lit) );
	lit.Type = D3DLIGHT_DIRECTIONAL;

	lit.Diffuse.r = c[0];
	lit.Diffuse.g = c[1];
	lit.Diffuse.b = c[2];
	lit.Diffuse.a = c[3];

	nsVec3	d = dir;
	d.Norm();
	lit.Direction.x = d.x;
	lit.Direction.y = d.y;
	lit.Direction.z = d.z;

	//lit.Range = 1000.0f;
	
	m_pdev->LightEnable( source, TRUE );
	m_pdev->SetLight( source, &lit );//*/	
}

void CRenderState::LightDisable( int source )
{
	m_pdev->LightEnable( source, FALSE );	
}

inline uint ColorToARGB( float r, float g, float b, float a )
{
	return (int(a * 255) << 24) | (int(r * 255) << 16) | (int(g * 255) << 8 ) | int(b * 255);
}

void CRenderState::SetLightAmbient( const float c[4] )
{
	//c[3] = 1;
	//COPY_COLOR( m_litAmb, c );
	
	m_pdev->SetRenderState( D3DRS_AMBIENT, ColorToARGB( c[0], c[1], c[2], 1 ) );
}
