#include "Input.h"
#include "nsLib/StrTools.h"
#include "nsLib/log.h"
#include "Core/Config.h"
#include "GameApp.h"
#include "Console.h"
#include "Platform.h"

nsInput		g_inp;

nsVar		*in_joy_use;

//---------------------------------------------------------
// IsKeyPressed: 
//---------------------------------------------------------
inline bool IsKeyPressed( int code )
{
	return App_GetPlatform()->IsKeyPressed(code);
}

//---------------------------------------------------------
// IsMouseKey: 
//---------------------------------------------------------
inline bool	IsMouseKey( int code )
{
	return code == 1 || code == 2;
}

//---------------------------------------------------------
// keycmd_f: 
//---------------------------------------------------------
void nsInput::keycmd_f( int argc, const char *argv[] )
{
	if ( argc < 2 )
	{
		LogPrintf( PRN_ALL, "usage: keycmd [keyname] [command/variable] [command params]\n" );
		return;
	}

	KeyBinding_t	*key = g_inp.GetKey( argv[1] );
	if ( !key )
	{
		LogPrintf( PRN_ALL, "invalid key name - '%s'\n", argv[1] );
		return;
	}

	if ( argc < 3 || strcmp( argv[2], "keycmd" ) == 0 )
	{
		switch ( key->type )
		{
		case IC_NONE:
			LogPrintf( PRN_ALL, "key '%s' unbound\n", argv[1] );
			break;
		case IC_VAR:
			LogPrintf( PRN_ALL, "key: '%s' variable: '%s'\n", argv[1], key->var->GetName() );
			break;
		case IC_CMD:
			LogPrintf( PRN_ALL, "key: '%s' command: '%s'\n", argv[1], key->cmd->name );
			if ( key->argc )
			{
				LogPrintf( PRN_ALL, "params count: %i\n", key->argc );
				for ( int i = 0; i < key->argc; i++ )
					LogPrintf( PRN_ALL, "[%i] -> '%s'\n", i, key->argv[i] );
			}
			break;
		}
		return;
	}

	nsVar	*var;
	const cmdDesc_t	*c;
	if ( (var = g_cfg->FindVar( argv[2][0] == '+' ? &argv[2][1] : argv[2] )) )
		g_inp.BindKeyVar( key, var, argv[2][0] == '+' );
	else if ( (c = g_cfg->FindCmd( argv[2][0] == '+' ? &argv[2][1] : argv[2] )) )
		g_inp.BindKeyCmd( key, c, argc - 2, &argv[2], argv[2][0] == '+' );
	else
		LogPrintf( PRN_ALL, "WARNING: '%s' - invalid variable or command!\n", argv[2] );
}//*/

//---------------------------------------------------------
// in_showkeys_f: 
//---------------------------------------------------------
void nsInput::in_showkeys_f( int argc, const char *argv[] )
{
	g_inp.ShowKeys();
}

//---------------------------------------------------------
// keydesc_f: 
//---------------------------------------------------------
void nsInput::keydesc_f( int argc, const char *argv[] )
{
	if ( argc < 2 )
	{
		LogPrintf( PRN_ALL, "usage: in_keydesc [keycode]\n" );
		return;
	}

	int			code = atoi( argv[1] );
	const char	*desc = g_inp.GetKeyDesc( code );
	LogPrintf( PRN_ALL, "code: %i desc: %s\n", code, desc );
}

//---------------------------------------------------------
// nsInput::nsInput: 
//---------------------------------------------------------
nsInput::nsInput()
{
	//memset( m_kbPrev, 0, sizeof(m_kbPrev) );//*/

	m_bAnyKey = false;
	m_joyUse = false;
	m_uiActive = false;

	nsConfig::AddWriter( this );
}

//---------------------------------------------------------
// nsInput::init: 
//---------------------------------------------------------
void nsInput::Init()
{
	g_cfg->RegCmd( "in_keydesc", keydesc_f );
	g_cfg->RegCmd( "keycmd", keycmd_f );
	g_cfg->RegCmd( "in_showkeys", in_showkeys_f );
	//g_cfg->RegCmd( "in_show_controllers", PrintControllers);
	in_joy_use = g_cfg->RegVar( "in_joy_use", "0", GVF_SAVABLE );

	memset( m_inKeys, 0, sizeof(m_inKeys) );
    for (int key = 0; key < KEYBOARD_MAX_KEYS; key++) {
        auto name = App_GetPlatform()->GetKeyName(key);
        if (name) {
            strcpy(m_inKeys[key].keyname, name);
        }
    }

	LoadBindings();
}

//---------------------------------------------------------
// nsInput::Release: 
//---------------------------------------------------------
void nsInput::Release()
{
	if ( m_joyUse )
	{
		LogPrintf( PRN_ALL, "...release joystick capture\n" );
		//joyReleaseCapture( 0 );
	}

	for (auto & m_inKey : m_inKeys)
		if ( m_inKey.type == IC_CMD && m_inKey.argv )
		{
			for ( int j = 0; j < m_inKey.argc; j++ )
				my_free( m_inKey.argv[j] );
			my_free( m_inKey.argv );
		}

	for (auto & m_joyKey : m_joyKeys)
		if ( m_joyKey.type == IC_CMD && m_joyKey.argv )
		{
			for ( int j = 0; j < m_joyKey.argc; j++ )
				my_free( m_joyKey.argv[j] );
			my_free( m_joyKey.argv );
		}
}

//---------------------------------------------------------
// nsInput::Process:
//---------------------------------------------------------
void nsInput::Process()
{
    auto platform = App_GetPlatform();
    m_bAnyKey = false;

    int mouseX, mouseY;
    platform->GetCursorPos(mouseX, mouseY);

    int winWidth, winHeight;
    platform->GetClientSize(winWidth, winHeight);

    if (nsMath::InRange(mouseX, 0, winWidth)
        && nsMath::InRange(mouseY, 0, winHeight)) {
        _cursorInClient = true;
        UpdateCursorState();
    } else {
        _cursorInClient = false;
        UpdateCursorState();
    }

    if (nsConsole::Shared()->IsActive()) {
        m_uiActive = true;
        return;
    }

    if (IsSystemKeyPressed()) return;

	bool	state;
	bool	any_key = false;

	Log::Info("======================= pressed");
	for ( int i = 0; i < KEYBOARD_MAX_KEYS; i++ )
	{
		state = IsKeyPressed( i );
		if (state) {
			Log::Info("pressed: %s (%i)", m_inKeys[i].keyname, i);
		}

		if ( m_uiActive && state )
		{
			any_key = true;
			break;
		}
		else if ( state )
		{
			if ( !m_kbPrev[i] )
			{
				KeyDown( &m_inKeys[i] );

				if ( !IsMouseKey( i ) )		m_bAnyKey = true;
				else if ( _cursorInClient )	m_bAnyKey = true;
			}

			KeyPressed( &m_inKeys[i] );
		}
		else if ( m_kbPrev[i] )
			KeyUp( &m_inKeys[i] );

		m_kbPrev[i] = state;
	}

	if ( m_joyUse && in_joy_use->Bool() )
	{
		bool	jkeys[8];
		GetJoyState( jkeys );
		for ( int i = 0; i < 8; i++ )
		{
			state = jkeys[i];
			if ( state && m_uiActive )
			{
				any_key = true;
				break;
			}
			else if ( state )
			{
				if ( !m_joyPrev[i] )
				{
					KeyDown( &m_joyKeys[i] );
					m_bAnyKey = true;
				}
			}
			else if ( m_joyPrev[i] )
				KeyUp( &m_joyKeys[i] );
			m_joyPrev[i] = state;
		}
	}

	if ( !any_key )	m_uiActive = false;
}

//---------------------------------------------------------
// nsInput::ReleaseAllKeys:
//---------------------------------------------------------
void nsInput::ReleaseAllKeys()
{
	for ( int i = 0; i < KEYBOARD_MAX_KEYS; i++ )
	{
		if ( m_kbPrev[i] )
			KeyUp( &m_inKeys[i] );
		m_kbPrev[i] = false;
	}

	if ( m_joyUse && in_joy_use->Bool() )
	{
		for ( int i = 0; i < 8; i++ )
		{
			if ( m_joyPrev[i] )
				KeyUp( &m_joyKeys[i] );
			m_joyPrev[i] = false;
		}
	}
}

//---------------------------------------------------------
// nsInput::Unbind: 
//---------------------------------------------------------
void nsInput::Unbind( const char *var_cmd )
{
	KeyBinding_t	*k = m_inKeys;
	bool	inc = false;
	if ( var_cmd[0] == '+' )
	{
		inc = true;
		var_cmd++;
	}

	for ( int i = 0; i < KEYBOARD_MAX_KEYS; i++, k++ )
	{
		if ( k->type == IC_CMD )
		{
			if ( strcmp( k->cmd->name, var_cmd ) == 0 )
				UnbindKey( k );
		}
		else if	( k->type == IC_VAR )
		{
			if ( strcmp( k->var->GetName(), var_cmd ) == 0 && k->inc == inc )
				UnbindKey( k );
		}
	}
}

//---------------------------------------------------------
// nsInput::GetBind: 
//---------------------------------------------------------
int nsInput::GetBind( const char *var_cmd, nsString **keys )
{
	int			count = 0;
	*keys = nullptr;

	bool	inc = false;
	if ( var_cmd[0] == '+' )
	{
		inc = true;
		var_cmd ++;
	}

	KeyBinding_t	*k = m_inKeys;
	for ( int i = 0; i < KEYBOARD_MAX_KEYS; i++, k++ )
	{
		if ( k->type == IC_CMD )
		{
			if ( StrEqual( k->cmd->name, var_cmd ) )
			{
				AddToArray( &*keys, count, nsString( k->keyname ) );
			}
		}
		else if	( k->type == IC_VAR )
		{
			if ( StrEqual( k->var->GetName(), var_cmd )
				&& k->inc == inc )
			{
				AddToArray( &*keys, count, nsString( k->keyname ) );
			}
		}
	}

	return count;
}

//---------------------------------------------------------
// nsInput::ShowCursorSys: 
//---------------------------------------------------------
void nsInput::ShowCursor(bool show) {
    if (_cursorVisible != show) {
        _cursorVisible = show;
        UpdateCursorState();
    }
}

//---------------------------------------------------------
// nsInput::SetCursorPos: 
//---------------------------------------------------------
void nsInput::SetCursorPos( float x, float y )
{
    auto platform = App_GetPlatform();


	int winWidth, winHeight;
	platform->GetClientSize(winWidth, winHeight);

	int	w, h;
	App_GetGame()->GetGUIDimension( w, h );

	int clientX, clientY;
	clientX = int( x * winWidth / float(w) );
	clientY = int( ( float(h) - y ) * winHeight / float(h) );

    platform->SetCursorPos(clientX, clientY);
}

//---------------------------------------------------------
// nsInput::GetCursorPos: 
//---------------------------------------------------------
void nsInput::GetCursorPos( float &x, float &y )
{
	int mouseX, mouseY;
    App_GetPlatform()->GetCursorPos(mouseX, mouseY);
    x = (float)mouseX;
    y = (float)mouseY;
}

//---------------------------------------------------------
// nsInput::CanDrawCursor: 
//---------------------------------------------------------
bool nsInput::CanDrawCursor()
{
	return !_cursorVisible;
}

//---------------------------------------------------------
// nsInput::GetKeyDesc: 
//---------------------------------------------------------
const char* nsInput::GetKeyDesc( int code )
{
	return m_inKeys[code].keyname;
}

//---------------------------------------------------------
// nsInput::GetKeyNameByDesc:
//---------------------------------------------------------
bool nsInput::HasKey(const char *desc)
{
	if ( desc && strlen( desc ) )
	{
		for (auto & key : m_inKeys)
		{
			if ( strlen(key.keyname)
				&& StrEqual( key.keyname, desc ) )
			{
				return true;
			}
		}
	}
	return false;
}

//---------------------------------------------------------
// nsInput::WaitAnyKey: 
//---------------------------------------------------------
bool nsInput::WaitAnyKey()
{
	return m_bAnyKey;
}

//---------------------------------------------------------
// nsInput::IsAnyKeyPressed: 
//---------------------------------------------------------
bool nsInput::IsAnyKeyPressed()
{
	for ( int i = 0; i < KEYBOARD_MAX_KEYS; i++ )
	{
		if ( IsKeyPressed( i ) )
			return true;
	}
	return false;
}

//---------------------------------------------------------
// nsInput::OnSaveConfig: 
//---------------------------------------------------------
void nsInput::OnSaveConfig( IDataWriter *out )
{
	//save keys bindings
	for (auto & m_inKey : m_inKeys)
	{
		KeyBinding_t	*key = &m_inKey;
		if ( key->type == IC_VAR )
			out->Printf( "keycmd %s %s%s\n", key->keyname, key->inc ? "+": "", key->var->GetName() );
		else if ( key->type == IC_CMD )
		{
			out->Printf( "keycmd %s %s%s", key->keyname, key->play ? "+": "", key->cmd->name );
			for ( int j = 1; j < key->argc; j++ )
				out->Printf( " %s", key->argv[j] );
			out->Printf( "\n" );
		}
	}
}

//---------------------------------------------------------
// nsInput::LoadBindings: 
//---------------------------------------------------------
void nsInput::LoadBindings()
{
	char *cfg = (char*)g_cfg->GetDefaultConfig();
	if ( cfg )
		LoadKeys( cfg );

	cfg = (char*)g_cfg->GetCurrentConfig();
	if ( cfg )
		LoadKeys( cfg );
}

//---------------------------------------------------------
// nsInput::LoadKeys: 
//---------------------------------------------------------
void nsInput::LoadKeys(char *str)
{
	nsString	line;
	char		*from = str;
	while ( (from = strstr( from, "keycmd" )) )
	{
		int	len = strcspn( from, "\n\r" );
		if ( len <= 0 ) len = nsString::MAX_SIZE;

		line.CopyFrom( from, len );
		g_cfg->ExecLine( line );

		from ++;
	}
}

//---------------------------------------------------------
// nsInput::BindKeyCmd: 
//---------------------------------------------------------
void nsInput::BindKeyCmd( KeyBinding_t *key, const cmdDesc_t *cmd, int argc, const char *argv[], bool play )
{
	UnbindKey( key );

	if ( argc && cmd )
	{
		if ( !(key->argv = (char**)my_malloc( sizeof(char*) * argc )) ) return;
		key->argc = argc;
		for ( int i = 0; i < argc; i++ )
		{
			key->argv[i] = (char*)my_malloc( sizeof(char) * (strlen( argv[i] ) + 1) );
			strcpy( key->argv[i], argv[i] );
		}

		key->type = IC_CMD;
		key->cmd = cmd;
		key->play = play;
	}
}

//---------------------------------------------------------
// nsInput::BindKeyVar: 
//---------------------------------------------------------
void nsInput::BindKeyVar(KeyBinding_t *key, nsVar *var, bool inc)
{
	UnbindKey( key );

	key->type = IC_VAR;
	key->var = var;
	key->inc = inc;
}

//---------------------------------------------------------
// nsInput::UnbindKey:
//---------------------------------------------------------
void nsInput::UnbindKey(KeyBinding_t *key)
{
	//if ( key->type )
	{
		if ( key->argv )
		{
			for ( int i = 0; i < key->argc; i++ )
				my_free( key->argv[i] );
			my_free( key->argv );
			key->argv = nullptr;
		}
		key->argc = 0;

		key->type = IC_NONE;
		key->var = nullptr;
		key->cmd = nullptr;
	}
}

//---------------------------------------------------------
// nsInput::ShowKeys: 
//---------------------------------------------------------
void nsInput::ShowKeys()
{
	LogPrintf( PRN_ALL, "------------------------- key names ------------------------\n" );
	int	count = 0;
	for (auto & m_inKey : m_inKeys)
		if ( strlen(m_inKey.keyname) )
		{
			LogPrintf( PRN_ALL, "%s\n", m_inKey.keyname );
/*
			if ( count > 4 )
			{
				LogPrintf( PRN_ALL, "\n" );
				count = -1;
			}
*/
			count ++;
		}
	LogPrintf( PRN_ALL, "\n------------------------------------------------------------\n" );
}

//---------------------------------------------------------
// nsInput::GetKey: 
//---------------------------------------------------------
KeyBinding_t* nsInput::GetKey( const char *name )
{
	if ( !name ) return nullptr;

	for (auto & m_inKey : m_inKeys)
	{
		if ( StrEqual( m_inKey.keyname, name ) )
			return &m_inKey;
	}

	for (auto & m_joyKey : m_joyKeys)
	{
		if ( StrEqual( m_joyKey.keyname, name ) )
			return &m_joyKey;
	}

	return nullptr;
}

//---------------------------------------------------------
// nsInput::KeyPressed: 
//---------------------------------------------------------
void nsInput::KeyPressed(KeyBinding_t *key)
{
	if ( key->type == IC_CMD && key->play )
		key->cmd->func( key->argc, (const char**)key->argv );
}

//---------------------------------------------------------
// nsInput::KeyUp: 
//---------------------------------------------------------
void nsInput::KeyUp(KeyBinding_t *key)
{
	if ( key->type == IC_VAR )
	{
		if ( key->inc )
            key->var->Dec();
		else
            key->var->Inc();
	}
}

//---------------------------------------------------------
// nsInput::KeyDown: 
//---------------------------------------------------------
void nsInput::KeyDown(KeyBinding_t *key)
{
	if ( key->type == IC_VAR )
	{
		if ( key->inc )
            key->var->Inc();
		else
            key->var->Dec();
	}
	else if ( key->type == IC_CMD )
		key->cmd->func( key->argc, (const char**)key->argv );
}

//---------------------------------------------------------
// nsInput::GetJoyState:
//---------------------------------------------------------
void nsInput::GetJoyState( bool keys[8] )
{
	/*JOYINFO	ji;
	if ( joyGetPos( 0, &ji ) != JOYERR_NOERROR ) return;
	keys[0] = ji.wXpos == 0;		//left
	keys[1] = ji.wXpos == 65535;	//right
	keys[2] = ji.wYpos == 65535;	//down
	keys[3] = ji.wYpos == 0;		//up
	keys[4] = ji.wButtons & JOY_BUTTON1 != 0;
	keys[5] = ji.wButtons & JOY_BUTTON2 != 0;
	keys[6] = ji.wButtons & JOY_BUTTON3 != 0;
	keys[7] = ji.wButtons & JOY_BUTTON4 != 0;*/
}

//---------------------------------------------------------
// nsInput::IsCtrlKey:
//---------------------------------------------------------
bool nsInput::IsSystemKeyPressed()
{
	return IsKeyPressed(NS_KEY_LEFT_ALT) && IsKeyPressed(NS_KEY_TAB);
}

void nsInput::UpdateCursorState() {
    App_GetPlatform()->ShowCursor(_cursorVisible || !_cursorInClient);
}

void nsInput::OnActivateApp(bool active) {
    if (!active) {
        _cursorInClient = false;
    }
    UpdateCursorState();
}
