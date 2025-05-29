// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Console.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "Console.h"
#include "nsLib/log.h"
#include "Core/Package.h"
#include "Core/Config.h"
#include "nsLib/color.h"
#include "RenManager.h"
#include "RenAux.h"
#include "Font.h"
#include "GameApp.h"
#include "KeyCodes.h"
#include "utils/AppUtils.h"
#include "renderer/font/FontsCache.h"

extern float g_frameTime;

nsVar	*con_line_step;
nsVar	*con_enable;

static float			textMargin = 5.0f;
static const nsVec2		textScale( 0.5f, 1.0f );
static const nsColor	cLine( 0.5, 0.5, 1, 1 );

#define	CON_LINE_OFFS	5


//---------------------------------------------------------
// exec_f:
//---------------------------------------------------------
void exec_f( int argc, const char *argv[] )
{
	if ( argc < 2 )
	{
		Log::Info( "usage: exec [filename]" );
		return;
	}

	nsFile	*file = g_pack.LoadFile( argv[1] );
	if ( !file ) return;

	char	*line = (char*)file->GetData();
	while ( true )
	{
		int	add = 0;
		char	*end = nullptr;
		if ( (end = strstr( line, "\r\n" )) )
		{
			add = 2;
			*end = 0;
		}
		else if ( (end = strchr( line, '\n' )) )
		{
			add = 1;
			*end = 0;
		}

		g_cfg->ExecLine( line );
		
		//log_printf( PRN_ALL, "exec: '%s'\n", line );
		if ( end )
		{
			*end = '\n';
			line = end + add;
		}
		else
			break;
	}

	g_pack.ReleaseFile( file );
}

//---------------------------------------------------------
// nsConsole::nsConsole:
//---------------------------------------------------------
nsConsole::nsConsole() : m_text{},
                         m_line(128),
                         m_pPrint(nullptr),
                         _renState(nullptr),
                         tex_offs{0, 0} {
	m_tex = nullptr;
	m_bActive = false;
	m_nLineUp = 0;

	Clear();
	Log::Shared()->AddPolicy(this);
	g_cfg->RegCmd("cls", clear_f);
}

//---------------------------------------------------------
// nsConsole::~nsConsole:
//---------------------------------------------------------
nsConsole::~nsConsole()
{
	Log::Shared()->RemovePolicy(this);
}

//---------------------------------------------------------
// nsConsole::DrawContent:
//---------------------------------------------------------
void nsConsole::Draw()
{
	if ( !m_bActive ) return;

	const auto renDev = nsRenDevice::Shared()->Device();

	tex_offs[0] += g_frameTime * 0.01f;
	tex_offs[1] = tex_offs[0];

	auto [width, height] = nsAppUtils::GetClientSize();
	float	h_con = float(height) / 2.0f;

    nsSpriteDesc ds;
    ds.pos = nsVec2(0, h_con);
    ds.size = nsVec2((float) width, h_con);
    ds.tex = m_tex;
    ds.color = nsColor::gray;
    if (m_tex) {
        ds.tex1 = nsVec2(0, 0);

        int tw, th;
        m_tex->GetSize(tw, th);
        ds.tex2 = nsVec2(width / (float)tw, h_con / (float)th);
    }
	renDev->StateApply(_renState);
    renDev->TextureTranform(tex_offs, nullptr);
    RX_DrawSprite(ds);
    renDev->TextureTranform(nullptr, nullptr);

	RX_DrawLine( nsVec2( 0, h_con ), nsVec2( float(width), h_con ), cLine );
	
	//draw text
	int	line_count = int((h_con - CON_LINE_OFFS)  / con_line_step->Value());

	const float *c = m_nLineUp ? nsColor::yellow : nsColor::white;

	char	*line = m_pPrint;
	if ( *(line - 1) == '\n' ) line -= 2;
	
	line = SkipLines( line, m_nLineUp );
	
	float	vertPos = (float)h_con + 5.0f;
	float	y = vertPos + con_line_step->Value();
	for ( int i = 0; i < line_count; i++ )
	{
		char	*prev = line;
		line = LineFromEnd( line );
		if ( !line )
		{
			DrawLine( m_text, y, prev - m_text - 1, c );
			break;
		}

		DrawLine( line, y, prev - line + 1, c );
		line -= 2;
		
		y += con_line_step->Value();
	}//*/

	//draw input line
	const char *str = m_line.GetLine();
	DrawLine( str, vertPos, strlen( str ), nsColor::white );
	DrawCursor( vertPos );

    auto font = nsFontsCache::Shared()->SysFont();

	//draw version
	auto version = App_GetGame()->GetVersionInfo();
	nsVec2 size;
	int length = strlen(version);
	font->GetSize(version, size, length);
	size *= textScale;

	nsVec2 pos((float)width - size.x - textMargin, vertPos);
	font->Draw(version, pos, textScale, nsColor::white, length);
}

//---------------------------------------------------------
// nsConsole::DrawCursor:
//---------------------------------------------------------
void nsConsole::DrawCursor( float y )
{
	static	float	cursorTime = 0;
	static	bool	cursorDraw = false;

	cursorTime += g_frameTime;
	if ( cursorTime >= 0.5f )
	{
		cursorDraw = !cursorDraw;
		cursorTime -= 0.5f;
	}

	if ( cursorDraw )
	{
        auto font = nsFontsCache::Shared()->SysFont();
		float	lineSize[2];
		font->GetSize( "_", lineSize, 0 );
		float	pos[2] = { textMargin + lineSize[0] * textScale.x * m_line.GetCursorPos(), y };
		font->Draw( "_", pos, textScale, nsColor::white, 0 );
	}
}

//---------------------------------------------------------
// nsConsole::SkipLines:
//---------------------------------------------------------
char* nsConsole::SkipLines( char *from, int count)
{
	for ( int i = 0; i < count; i++ )
	{
		from = LineFromEnd( from );
		if ( !from ) return 0;

		from -= 2;
	}

	return from;
}

//---------------------------------------------------------
// nsConsole::LineFromEnd:
//---------------------------------------------------------
char* nsConsole::LineFromEnd(char *end)
{
	while ( end > m_text )
	{
		if ( *end == '\n' )
		{
			return end + 1;
		}
		end --;
	}

	return 0;
}

//---------------------------------------------------------
// nsConsole::LoadRes:
//---------------------------------------------------------
bool nsConsole::OnInit()
{
	nsSubSystem::OnInit();

	auto renDev = nsRenDevice::Shared()->Device();

	Log::Info( "init console" );
	m_tex = renDev->TextureLoad( "default/console.jpg", false, TF_RGB );
	_renState = renDev->StateLoad("default/rs/gui.txt");
	con_line_step = g_cfg->RegVar( "con_line_step", "20.0", GVF_SAVABLE );
	con_enable = g_cfg->RegVar( "con_enable", "1", 0 );
	g_cfg->RegCmd( "exec", exec_f );
		
	return true;
}

//---------------------------------------------------------
// nsConsole::FreeRes:
//---------------------------------------------------------
void nsConsole::OnRelease()
{
	nsSubSystem::OnRelease();
	Log::Info( "shutdown console" );
	if ( m_tex ) g_renDev->TextureRelease( (ITexture*)m_tex );
}

//---------------------------------------------------------
// nsConsole::print:
//---------------------------------------------------------
void nsConsole::LogPrint(LogLevel level, const char *str)
{
	int	rmn = CON_TEXT_SIZE - (m_pPrint - m_text) - 1;
	int	len = strlen( str );

	if ( len >= rmn ) Clear();

	strcat( m_pPrint, str );	
	m_pPrint += len;

	if ( m_nLineUp )
	{
		while ( *str )
		{
			if ( *str == '\n' ) m_nLineUp ++;
			str ++;
		}
	}
}

//---------------------------------------------------------
// nsConsole::Clear:
//---------------------------------------------------------
void nsConsole::Clear()
{
	m_text[0] = 0;
	m_pPrint = m_text;
}

//---------------------------------------------------------
// nsConsole::DrawLine:
//---------------------------------------------------------
void nsConsole::DrawLine( const char *line, float y, int len, const nsColor &c )
{
	if ( len > 0 )
	{
        auto font = nsFontsCache::Shared()->SysFont();
		float pos[2] = { textMargin, y };
		font->Draw( line, pos, textScale, c, len );
	}
}

//---------------------------------------------------------
// nsConsole::IsActive:
//---------------------------------------------------------
bool nsConsole::IsActive()
{
	return m_bActive;
}

//---------------------------------------------------------
// nsConsole::IsEnabled: 
//---------------------------------------------------------
bool nsConsole::IsEnabled()
{
	return con_enable->Value() != 0;
}

//---------------------------------------------------------
// nsConsole::Toggle:
//---------------------------------------------------------
void nsConsole::Toggle()
{
	Activate( !m_bActive );
}

//---------------------------------------------------------
// nsConsole::Activate:
//---------------------------------------------------------
void nsConsole::Activate(bool active)
{
	if ( m_bActive == active ) return;
	m_bActive = active;
	ClearLine();
}

//---------------------------------------------------------
// nsConsole::OnChar:
//---------------------------------------------------------
void nsConsole::OnChar(char ch)
{
	if ( iscntrl( ch ) || ch == '~' || ch == '`' ) return;
	m_line.TypeChar( ch );
}

//---------------------------------------------------------
// nsConsole::OnKeyDown:
//---------------------------------------------------------
void nsConsole::OnKeyDown( int key, bool rept )
{
	switch ( key )
	{
	case NS_KEY_BACKSPACE:
		m_line.KeyBackspace();
		break;
	case NS_KEY_DELETE:
		m_line.KeyDelete();
		break;
	case NS_KEY_LEFT:
		m_line.KeyLeft();
		break;
	case NS_KEY_RIGHT:
		m_line.KeyRight();
		break;
	case NS_KEY_ESCAPE:
		ClearLine();
		break;
	case NS_KEY_UP:
		m_line.SetLine( m_hyst.GetPrev() );
		break;
	case NS_KEY_DOWN:
		m_line.SetLine( m_hyst.GetCurr() );
		break;
	case NS_KEY_PAGE_UP:
		m_nLineUp ++;
		break;
	case NS_KEY_PAGE_DOWN:
		m_nLineUp --;
		if ( m_nLineUp < 0 ) m_nLineUp = 0;
		break;
	case NS_KEY_END:
		m_nLineUp = 0;
		break;
	case NS_KEY_TAB:
		{
			if ( !strlen( m_line.GetLine() ) ) return;
			const char	*str = g_cfg->CompleteLine( m_line.GetLine() );
			if ( str ) m_line.SetLine( str );
		}
		break;

	case NS_KEY_ENTER:
		{
			size_t len = strlen( m_line.GetLine() );
			if ( len > 0 )
			{
				Log::Info( "> %s", m_line.GetLine() );
				nsString line = m_line.GetLine();
				m_hyst.Add( line );

				g_cfg->ExecLine( m_line.GetLine() );
				
				ClearLine();
			}
		}
		break;
	}
}

void nsConsole::OnMouseWheel(float delta) {
    m_nLineUp += (int)delta;
    if ( m_nLineUp < 0 ) m_nLineUp = 0;
}

//---------------------------------------------------------
// nsConsole::ClearLine:
//---------------------------------------------------------
void nsConsole::ClearLine()
{
	m_line.SetLine( "" );
}

void nsConsole::clear_f(int argc, const char **argv) {
    Shared()->Clear();
}

void nsConsole::OnPointerCancel(int pointerId) {

}

