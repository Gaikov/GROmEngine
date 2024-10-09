#include "ConfigDlg.h"
#include "resource.h"
#include "Local.h"
#include "nsLib/log.h"
#include "Core/Package.h"
#include "Core/Config.h"
#include "Core/Parser.h"
#include "nsLib/StrTools.h"

typedef struct
{
	uint	id;
	const char	*name;
}
itemNameRes_t;

itemNameRes_t	nameRes[] = 
{
	{ IDC_DEVICE_STATIC, "device" },
	{ IDC_VIDMODE_STATIC, "vidmode_grp" },
	{ IDC_MODE_STATIC, "mode" },
	{ IDC_COLOR_DEPTH_STATIC, "color_depth" },
	{ IDC_FULLSCREEN_CHECK, "fullscreen" },
	{ IDC_VSYNC_CHECK, "vsync" },
	{ IDC_TEXTURE_STATIC, "texture_grp" },
	{ IDC_TEX_DEPTH_STATIC, "tex_depth" },
	{ IDC_TEX_FILTER_STATIC, "tex_filter" },
	{ IDC_DEFAULT_BUTTON, "default" },
	{ IDOK, "ok" },
	{ IDCANCEL, "cancel" },
};

int nameResCount = sizeof(nameRes) / sizeof(itemNameRes_t);

CConfigDlg dlgCfg;	//����� ����������� dll �������� ������ ���� CConfigDlg ��� ������ ����������

//---------------------------------------------------------
// CConfigDlg::CConfigDlg: 
//---------------------------------------------------------
CConfigDlg::CConfigDlg() :
	m_localSS( 0 )
{
	m_pd3d = 0;
	m_currPixelBits = 0;
}

//---------------------------------------------------------
// CConfigDlg::~CConfigDlg: 
//---------------------------------------------------------
CConfigDlg::~CConfigDlg()
{
	m_pd3d = 0;
}

//---------------------------------------------------------
// CConfigDlg::CallBackProc: 
//---------------------------------------------------------
BOOL CConfigDlg::CallBackProc(UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//---------------------------------------------------------
// CConfigDlg::OnInitDialog: 
//---------------------------------------------------------
int CConfigDlg::OnInitDialog()
{
	LogPrintf( PRN_ALL, "--- init config dialog ---\n" );

	r_d3d_device = g_cfg->RegVar( "r_d3d_device", "0", GVF_SAVABLE | GVF_READONLY );

	LogPrintf( PRN_ALL, "...create Direct3D\n" );
	m_pd3d = Direct3DCreate8( D3D_SDK_VERSION );
	if ( !m_pd3d )
	{
		MessageBox( "ERROR: can't create Direct3D", "D3D ERROR" );
		return 0;
	}

	D3DDISPLAYMODE	d3dmode;
	memset( &d3dmode, 0, sizeof(d3dmode) );
	m_pd3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3dmode );
	fmtdesc_t *fmt = d3d_GetFormatDesc( d3dmode.Format );
	if ( fmt ) m_currPixelBits = fmt->bits;

	UpdateView();
	UpdateCtrlLocalText();
	
	return 1;
}

//---------------------------------------------------------
// CConfigDlg::OnCommand: 
//---------------------------------------------------------
int CConfigDlg::OnCommand(int control, int notify)
{
	switch ( control )
	{
	case IDOK:
		return OnOK();
	case IDCANCEL:
		return OnCancel();
	case IDC_DEVICE_COMBO:
		if ( notify == CBN_SELCHANGE )
			OnChangeDevice();
		break;
	case IDC_FULLSCREEN_CHECK:
		//UpdateColorBits();
		OnFullScreenCheck();
		break;
	case IDC_DEFAULT_BUTTON:
		return OnDefaultButton();
		break;
	}

	return 0;
}

int CConfigDlg::OnCancel()
{
	EndDialog( m_hDlg, IDOK );
	OnClose();
	return 0;
}

int CConfigDlg::OnOK()
{
	r_vsync->SetValue( (float)check_GetChecked( IDC_VSYNC_CHECK ) );
	r_pixel_bits->SetValue( cbox_GetSelected( IDC_VID_COLOR_COMBO ) ? 32.0f : 16.0f );
	r_fullscreen->SetValue( (float)check_GetChecked( IDC_FULLSCREEN_CHECK ) );
	r_tex_bits->SetValue( cbox_GetSelected( IDC_TEX_COLOR_COMBO ) ? 32.0f : 16.0f );
	r_tex_filter->SetValue( (float)cbox_GetSelected( IDC_TEX_FILTER_COMBO ) );

	int	index = cbox_GetSelected( IDC_VIDMODE_COMBO );
	mode_desc_t	*md = modes.GetMode( index );
	r_width->SetValue( (float)md->width );
	r_height->SetValue( (float)md->height );
	r_refresh->SetValue( (float)md->refresh );

	r_d3d_device->SetValue( (float)cbox_GetSelected( IDC_DEVICE_COMBO ) );

	EndDialog( m_hDlg, IDCANCEL );
	OnClose();
	return 0;
}

void CConfigDlg::MessageBox(char *message, char *title)
{
	LogPrintf( PRN_ALL, "%s\n", message );
	::MessageBox( m_hDlg, message, title, MB_OK | MB_ICONINFORMATION );
}

//---------------------------------------------------------
// CConfigDlg::OnClose: 
//---------------------------------------------------------
void CConfigDlg::OnClose()
{
	LogPrintf( PRN_ALL, "--- close config dialog ---\n" );
	if ( m_pd3d )
	{
		LogPrintf( PRN_ALL, "...release Direct3D\n" );
		m_pd3d->Release();
		m_pd3d = 0;
	}

	modes.Clear();

	if ( m_localSS )
	{
		ps_end( m_localSS );
		m_localSS = 0;
	}

	if ( m_localFile )
	{
		g_pack.ReleaseFile( m_localFile );
		m_localFile = 0;
	}
}

//---------------------------------------------------------
// CConfigDlg::OnChangeDevice: 
//---------------------------------------------------------
void CConfigDlg::OnChangeDevice()
{
	UpdateModeList( (int)r_width->Value(), (int)r_height->Value() );
}

//---------------------------------------------------------
// CConfigDlg::UpdateModeList: 
//---------------------------------------------------------
void CConfigDlg::UpdateModeList( int width, int height )
{
	modes.Clear();
	d3d_EnumModes( modes, cbox_GetSelected( IDC_DEVICE_COMBO ), m_pd3d );

	mode_desc_t	*md;
	cbox_Clear( IDC_VIDMODE_COMBO );
	for ( int i = 0; i < modes.count; i++ )
	{
		md = modes.GetMode( i );
		if ( md )
			cbox_AddString( IDC_VIDMODE_COMBO, StrPrintf( "%ix%i (%i Hz)", md->width, md->height, md->refresh ) );
	}

	//select mode
	int	index = modes.FindUserMode( width, height );
	if ( index < 0 ) index = 0;
	cbox_SetSelected( IDC_VIDMODE_COMBO, index );
	md = modes.GetMode( index );

	r_width->SetValue( (float)md->width );
	r_height->SetValue( (float)md->height );
	r_refresh->SetValue( (float)md->refresh );
}

//---------------------------------------------------------
// CConfigDlg::UpdateColorBits: 
//---------------------------------------------------------
void CConfigDlg::UpdateColorBits()
{
	int	fs = check_GetChecked( IDC_FULLSCREEN_CHECK );
	cbox_Clear( IDC_VID_COLOR_COMBO );
	cbox_AddString( IDC_VID_COLOR_COMBO, "16" );
	cbox_AddString( IDC_VID_COLOR_COMBO, "32" );
	
	/*if ( r_pixel_bits->value == 32 )
		cbox_SetSelected( IDC_VID_COLOR_COMBO, 1 );
	else
	{
		cbox_SetSelected( IDC_VID_COLOR_COMBO, 0 );
		ri.g_cfg->SetValue( "r_pixel_bits", 16 );
	}//*/
	if ( fs )
		cbox_SetSelected( IDC_VID_COLOR_COMBO, r_pixel_bits->Value() == 32 ? 1 : 0 );
	else
		cbox_SetSelected( IDC_VID_COLOR_COMBO, m_currPixelBits == 32 ? 1 : 0 );

	ctrl_Enable( IDC_VID_COLOR_COMBO, fs );
}

void CConfigDlg::UpdateCheckBoxes()
{
	check_SetChecked( IDC_FULLSCREEN_CHECK, (int)r_fullscreen->Value() );
	check_SetChecked( IDC_VSYNC_CHECK, (int)r_vsync->Value() );
}

//---------------------------------------------------------
// CConfigDlg::UpdateTextureFmt: 
//---------------------------------------------------------
void CConfigDlg::UpdateTextureFmt()
{
	cbox_Clear( IDC_TEX_COLOR_COMBO );
	cbox_AddString( IDC_TEX_COLOR_COMBO, "16" );
	cbox_AddString( IDC_TEX_COLOR_COMBO, "32" );
	if ( r_tex_bits->Value() == 32 )
		cbox_SetSelected( IDC_TEX_COLOR_COMBO, 1 );
	else
	{
		cbox_SetSelected( IDC_TEX_COLOR_COMBO, 0 );
		r_tex_bits->SetValue( 16 );
	}

	cbox_Clear( IDC_TEX_FILTER_COMBO );

	const char	*str;
	str = ps_get_str( m_localSS, "bilinear" );
	cbox_AddString( IDC_TEX_FILTER_COMBO, str ? str : "Bilinear" );
	str = ps_get_str( m_localSS, "trilinear" );
	cbox_AddString( IDC_TEX_FILTER_COMBO, str ? str : "Trilinear" );
	
	if ( r_tex_filter->Value() == 1 )
		cbox_SetSelected( IDC_TEX_FILTER_COMBO, 1 );
	else
	{
		cbox_SetSelected( IDC_TEX_FILTER_COMBO, 0 );
		r_tex_filter->SetValue( 0 );
	}
}

//---------------------------------------------------------
// CConfigDlg::SetLocalDesc: 
//---------------------------------------------------------
void CConfigDlg::SetLocalDesc( const char *path, const char *postfix )
{
	if ( !path || !postfix ) return;
	
	nsString	localFile;
	localFile.Format( "%s/d3d8_%s.txt", path, postfix );

	m_localFile = g_pack.LoadFile( localFile.AsChar() );
	if ( !m_localFile )
	{
		LogPrintf( PRN_ALL, "WARNING: can't open d3d local desc '%s'\n", localFile.AsChar() );
		return;
	}

	m_localSS = ps_begin( (char*)m_localFile->GetData(), '$' );
	if ( !ps_block_begin( m_localSS, "d3d_dialog" ) )
	{
		LogPrintf( PRN_ALL, "WARNING: invalid local desc '%s'\n", localFile.AsChar() );
		g_pack.ReleaseFile( m_localFile ), m_localFile = 0;
		ps_end( m_localSS ), m_localSS = 0;
	}
}

//-----------------------------------------------------
//  CConfigDlg::OnDefaultButton:  
//-----------------------------------------------------
int CConfigDlg::OnDefaultButton()
{
	int	fs = int( r_fullscreen->GetDefaultValue() );
	check_SetChecked( IDC_FULLSCREEN_CHECK, fs );
	cbox_SetSelected( IDC_TEX_COLOR_COMBO, int( r_tex_bits->GetDefaultValue() ) == 32 ? 1 : 0 );
	check_SetChecked( IDC_VSYNC_CHECK, int( r_vsync->GetDefaultValue() ) );
	cbox_SetSelected( IDC_TEX_FILTER_COMBO, int( r_tex_filter->GetDefaultValue() ) );
	cbox_SetSelected( IDC_DEVICE_COMBO, int( r_d3d_device->GetDefaultValue() ) );

	UpdateModeList( int( r_width->GetDefaultValue() ), int( r_height->GetDefaultValue() ) );
	
	if ( fs )
		cbox_SetSelected( IDC_VID_COLOR_COMBO, int( r_pixel_bits->GetDefaultValue() ) == 32 ? 1 : 0 );
	else
		cbox_SetSelected( IDC_VID_COLOR_COMBO, m_currPixelBits == 32 ? 1 : 0 );
	ctrl_Enable( IDC_VID_COLOR_COMBO, fs );
	return 0;	
}

//-----------------------------------------------------
//  CConfigDlg::UpdateView:  
//-----------------------------------------------------
bool CConfigDlg::UpdateView()
{
	HRESULT	hr;
	cbox_Clear( IDC_DEVICE_COMBO );
	D3DADAPTER_IDENTIFIER8	did;
	int count = (int)m_pd3d->GetAdapterCount();
	for ( int i = 0; i < (int)count; i++ )
	{
		hr = m_pd3d->GetAdapterIdentifier( i, D3DENUM_NO_WHQL_LEVEL, &did );
		if ( FAILED(hr) )
		{
			d3d_FatalError( "GetAdapterIdentifier", hr );
			return false;
		}
		cbox_AddString( IDC_DEVICE_COMBO, did.Description );
	}

	if ( r_d3d_device->Value() < 0 || r_d3d_device->Value() >= count )
		r_d3d_device->SetValue( 0 );

	cbox_SetSelected( IDC_DEVICE_COMBO, (int)r_d3d_device->Value() );

	UpdateModeList( (int)r_width->Value(), (int)r_height->Value() );
	UpdateCheckBoxes();
	UpdateColorBits();
	UpdateTextureFmt();

	return true;
}


//---------------------------------------------------------
// CConfigDlg::UpdateCtrlLocalText: 
//---------------------------------------------------------
void CConfigDlg::UpdateCtrlLocalText()
{
	if ( !m_localSS ) return;

	const char *cap = ps_get_str( m_localSS, "caption" );
	if ( cap ) SetWindowText( m_hDlg, cap );

	for ( int i = 0; i < nameResCount; i++ )
		ctrl_SetText( nameRes[i].id, ps_get_str( m_localSS, nameRes[i].name ) );
}

//---------------------------------------------------------
// CConfigDlg::OnFullScreenCheck: 
//---------------------------------------------------------
void CConfigDlg::OnFullScreenCheck()
{
	int	fs = check_GetChecked( IDC_FULLSCREEN_CHECK );
	ctrl_Enable( IDC_VID_COLOR_COMBO, fs );
	if ( !fs )
		cbox_SetSelected( IDC_VID_COLOR_COMBO, m_currPixelBits == 32 ? 1 : 0 );
}
