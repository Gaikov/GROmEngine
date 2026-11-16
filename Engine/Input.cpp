#include "Input.h"
#include <algorithm>
#include <charconv>
#include <cstring>
#include <string>
#include <string_view>
#include "nsLib/StrTools.h"
#include "nsLib/log.h"
#include "Core/Config.h"
#include "GameApp.h"
#include "console/Console.h"
#include "Platform.h"

nsInput		g_inp;


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

namespace {
// Bound a malformed console/config index before allocating an indexed table.
constexpr int MAX_GAMEPAD_BINDINGS = 65536;

bool ParseButtonIndex(const char* text, int& index) {
    const auto end = text + std::strlen(text);
    const auto result = std::from_chars(text, end, index);
    return result.ec == std::errc{} && result.ptr == end
        && index >= 0 && index < MAX_GAMEPAD_BINDINGS;
}
}

//---------------------------------------------------------
// keycmd_f: 
//---------------------------------------------------------
void nsInput::keycmd_f( int argc, const char *argv[] )
{
	if ( argc < 2 )
	{
		Log::Info("usage: keycmd [keyname] [command/variable] [command params]" );
		return;
	}

	KeyBinding_t	*key = g_inp.GetKey( argv[1] );
	if ( !key )
	{
		Log::Info("invalid key name - '%s'", argv[1] );
		return;
	}

	if ( argc < 3 || strcmp( argv[2], "keycmd" ) == 0 )
	{
		switch ( key->type )
		{
		case IC_NONE:
			Log::Info("key '%s' unbound", argv[1] );
			break;
		case IC_VAR:
			Log::Info("key: '%s' variable: '%s'", argv[1], key->var->GetName() );
			break;
		case IC_CMD:
			Log::Info("key: '%s' command: '%s'", argv[1], key->cmd->name );
			if ( key->argc )
			{
				Log::Info("params count: %i", key->argc );
				for ( int i = 0; i < key->argc; i++ )
					Log::Info("[%i] -> '%s'", i, key->argv[i] );
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
		Log::Warning("'%s' - invalid variable or command!", argv[2] );
}//*/

//--- nsInput::PadCmd: ---
void nsInput::PadCmd(int argc, const char* argv[]) {
    if (argc < 2) {
        Log::Info("usage: padcmd [button index] [command/variable|none] [command params]");
        return;
    }
    int index = 0;
    if (!ParseButtonIndex(argv[1], index)) {
        Log::Warning("invalid gamepad button index '%s' (expected 0..%d)", argv[1], MAX_GAMEPAD_BINDINGS - 1);
        return;
    }
    if (argc < 3) {
        Log::Info("gamepad button %d:", index);
        PrintBinding(index < static_cast<int>(g_inp._gamepadBindings.size()) ? &g_inp._gamepadBindings[index] : nullptr);
        return;
    }
    KeyBinding_t replacement{};
    if (std::strcmp(argv[2], "none") == 0) {
        if (argc != 3) {
            Log::Warning("usage: padcmd [button index] none");
            return;
        }
    } else {
        const auto positive = argv[2][0] == '+';
        const auto name = positive ? argv[2] + 1 : argv[2];
        if (auto var = g_cfg->FindVar(name)) g_inp.BindKeyVar(&replacement, var, positive);
        else if (auto cmd = g_cfg->FindCmd(name)) {
            if (!g_inp.BindKeyCmd(&replacement, cmd, argc - 2, argv + 2, positive)) return;
        } else {
            Log::Warning("'%s' - invalid variable or command!", argv[2]);
            return;
        }
    }
    replacement.configured = true;

    // Preparing the replacement before touching the old binding also permits
    // a command to rebind itself using its current argv.
    try {
        if (index >= static_cast<int>(g_inp._gamepadBindings.size())) g_inp._gamepadBindings.resize(index + 1);
    } catch (...) {
        if (replacement.type == IC_CMD) FreeArgs({replacement.argc, replacement.argv});
        throw;
    }
    g_inp.CancelGamepadBinding(index);
    g_inp.UnbindKey(&g_inp._gamepadBindings[index]);
    g_inp._gamepadBindings[index] = replacement;
}

void nsInput::PrintBinding(const KeyBinding_t* binding) {
    if (!binding || binding->type == IC_NONE) Log::Info("unbound");
    else if (binding->type == IC_VAR) Log::Info("variable: %s%s", binding->inc ? "+" : "", binding->var->GetName());
    else {
        Log::Info("command: %s%s", binding->play ? "+" : "", binding->cmd->name);
        for (auto i = 1; i < binding->argc; ++i) Log::Info("arg %d: %s", i, binding->argv[i]);
    }
}

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
		Log::Info("usage: in_keydesc [keycode]" );
		return;
	}

	int			code = atoi( argv[1] );
	const char	*desc = g_inp.GetKeyDesc( code );
	Log::Info("code: %i desc: %s", code, desc );
}

//---------------------------------------------------------
// nsInput::nsInput: 
//---------------------------------------------------------
nsInput::nsInput()
{
	//memset( m_kbPrev, 0, sizeof(m_kbPrev) );//*/

	m_bAnyKey = false;
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
    g_cfg->RegCmd("padcmd", PadCmd);
    // Retained for legacy configs/UI; raw padcmd bindings ignore it.
    g_cfg->RegVar("in_joy_use", "0", GVF_SAVABLE);
    _appActive = true;
    m_uiActive = m_bAnyKey = false;
    std::fill_n(m_kbPrev, KEYBOARD_MAX_KEYS, false);
    std::fill_n(m_awaitRelease, KEYBOARD_MAX_KEYS, false);
    _varPresses.clear();
    _varPresses.reserve(KEYBOARD_MAX_KEYS);

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
void nsInput::Release() {
    // Shutdown also runs after a failed init, before a native window exists.
    // Balance recorded contributions without polling or rearming controls.
    ++_gamepadResetGeneration;
    _varPresses.clear();
    m_bAnyKey = false;
    for (auto i = 0; i < KEYBOARD_MAX_KEYS; ++i) {
        if (m_kbPrev[i] && !m_awaitRelease[i]) KeyUp(&m_inKeys[i]);
        m_kbPrev[i] = m_awaitRelease[i] = false;
    }
    for (auto& state : _gamepads) ReleaseGamepad(state);
    for (auto& key : m_inKeys) UnbindKey(&key);
    for (auto& key : _gamepadBindings) UnbindKey(&key);
    _gamepadBindings.clear();
    _gamepads.clear();
}

//---------------------------------------------------------
// nsInput::Process:
//---------------------------------------------------------
void nsInput::Process()
{
    auto platform = App_GetPlatform();
    m_bAnyKey = false;
    _varPresses.clear();
    SyncGamepads(); // Detach cleanup also runs while the console blocks input.

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

    if (GamepadsBlocked()) CancelGamepads();
    if (nsConsole::Shared()->IsActive()) {
        m_uiActive = true;
        return;
    }

    if (IsSystemKeyPressed()) return;

	bool	state;
	bool	any_key = false;
    const auto generation = _gamepadResetGeneration;

	for ( int i = 0; i < KEYBOARD_MAX_KEYS; i++ )
	{
		state = IsKeyPressed( i );

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
                if (_gamepadResetGeneration != generation) return;

				if ( !IsMouseKey( i ) )		m_bAnyKey = true;
				else if ( _cursorInClient )	m_bAnyKey = true;
			}

			KeyPressed( &m_inKeys[i] );
            if (_gamepadResetGeneration != generation) return;
		}
		else if ( m_kbPrev[i] )
		{
			if ( !m_awaitRelease[i] )
				KeyUp( &m_inKeys[i] );
			m_awaitRelease[i] = false;
		}

		m_kbPrev[i] = state;
	}


	if ( !any_key )	m_uiActive = false;
    ProcessGamepads();
}

nsInput::nsGamepadState* nsInput::FindGamepad(uint64_t connectionId) {
    for (auto& state : _gamepads) if (state.connectionId == connectionId) return &state;
    return nullptr;
}

const IGamepad* nsInput::FindPlatformGamepad(uint64_t connectionId) {
    const auto platform = App_GetPlatform();
    for (auto i = 0; i < platform->GetGamepadCount(); ++i) {
        const auto pad = platform->GetGamepad(i);
        if (pad && pad->GetConnectionId() == connectionId) return pad;
    }
    return nullptr;
}

void nsInput::ReleaseGamepad(nsGamepadState& state) {
    for (auto i = size_t(0); i < state.buttons.size(); ++i) {
        if (state.buttons[i].previous && !state.buttons[i].awaitRelease && i < _gamepadBindings.size()) KeyUp(&_gamepadBindings[i]);
        state.buttons[i].previous = false;
    }
}

void nsInput::SyncGamepads() {
    for (auto& state : _gamepads) state.seen = false;
    const auto platform = App_GetPlatform();
    auto pressCapacity = size_t(KEYBOARD_MAX_KEYS);
    for (auto i = 0; i < platform->GetGamepadCount(); ++i) {
        const auto pad = platform->GetGamepad(i);
        if (!pad) continue;
        const auto count = static_cast<size_t>(std::max(0, pad->GetButtonCount()));
        pressCapacity += count;
        auto state = FindGamepad(pad->GetConnectionId());
        if (!state) {
            _gamepads.push_back({pad->GetConnectionId(), false, {}});
            state = &_gamepads.back();
        }
        if (state->buttons.size() != count) {
            ReleaseGamepad(*state);
            state->buttons.assign(count, {});
            for (auto button = size_t(0); button < count; ++button) {
                const auto down = pad->IsButtonPressed(static_cast<int>(button));
                state->buttons[button].previous = down;
                state->buttons[button].awaitRelease = down;
            }
        }
        state->seen = true;
    }
    for (auto it = _gamepads.begin(); it != _gamepads.end();) {
        if (!it->seen) {
            ReleaseGamepad(*it);
            it = _gamepads.erase(it);
        } else ++it;
    }
    _varPresses.reserve(pressCapacity);
}

bool nsInput::GamepadsBlocked() const {
    return !_appActive || nsConsole::Shared()->IsActive() || IsSystemKeyPressed();
}

void nsInput::CancelGamepads() {
    ++_gamepadResetGeneration;
    for (auto& state : _gamepads) {
        ReleaseGamepad(state);
        const auto pad = FindPlatformGamepad(state.connectionId);
        for (auto i = size_t(0); i < state.buttons.size(); ++i) {
            const auto down = pad && pad->IsButtonPressed(static_cast<int>(i));
            state.buttons[i].previous = down;
            state.buttons[i].awaitRelease = down;
        }
    }
}

void nsInput::CancelGamepadBinding(int index) {
    for (auto& state : _gamepads) {
        if (index >= static_cast<int>(state.buttons.size())) continue;
        auto& button = state.buttons[index];
        if (button.previous && !button.awaitRelease) KeyUp(&_gamepadBindings[index]);
        const auto pad = FindPlatformGamepad(state.connectionId);
        button.previous = pad && pad->IsButtonPressed(index);
        button.awaitRelease = button.previous;
    }
}

//--- nsInput::ProcessGamepads: ---
void nsInput::ProcessGamepads() {
    if (GamepadsBlocked()) {
        CancelGamepads();
        _varPresses.clear();
        return;
    }
    const auto platform = App_GetPlatform();
    const auto generation = _gamepadResetGeneration;
    for (auto i = 0; i < platform->GetGamepadCount(); ++i) {
        const auto pad = platform->GetGamepad(i);
        if (!pad) continue;
        const auto state = FindGamepad(pad->GetConnectionId());
        if (!state) continue;
        for (auto index = size_t(0); index < state->buttons.size(); ++index) {
            auto& button = state->buttons[index];
            const auto down = pad->IsButtonPressed(static_cast<int>(index));
            const auto wasDown = button.previous;
            button.previous = down;
            if (!down) {
                if (wasDown && !button.awaitRelease && index < _gamepadBindings.size()) KeyUp(&_gamepadBindings[index]);
                button.awaitRelease = false;
                continue;
            }
            if (button.awaitRelease) continue;
            const auto pressed = !wasDown;
            if (pressed) m_bAnyKey = true;
            if (index >= _gamepadBindings.size()) continue;
            if (pressed) KeyDown(&_gamepadBindings[index]);
            else KeyPressed(&_gamepadBindings[index]);
            // Never retain a binding reference across callbacks: padcmd can resize it.
            if (_gamepadResetGeneration != generation) return;
            if (GamepadsBlocked()) {
                CancelGamepads();
                _varPresses.clear();
                m_bAnyKey = false;
                return;
            }
        }
    }
}

//---------------------------------------------------------
// nsInput::ReleaseAllKeys:
//---------------------------------------------------------
void nsInput::ReleaseAllKeys()
{
    _varPresses.clear();
    m_bAnyKey = false;
	for ( int i = 0; i < KEYBOARD_MAX_KEYS; i++ )
	{
		if ( m_kbPrev[i] && !m_awaitRelease[i] )
			KeyUp( &m_inKeys[i] );
		m_kbPrev[i] = IsKeyPressed( i );
		m_awaitRelease[i] = m_kbPrev[i];
	}

    CancelGamepads();
}

//--- nsInput::GetVarPressDirection: ---
int nsInput::GetVarPressDirection(const nsVar *var) const {
    for (auto i = _varPresses.size(); i > 0; --i) {
        const auto &press = _varPresses[i - 1];
        if (press.var == var) return press.direction;
    }
    return 0;
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
    const auto platform = App_GetPlatform();
    for (auto i = 0; i < platform->GetGamepadCount(); ++i) {
        const auto pad = platform->GetGamepad(i);
        if (pad) {
            for (auto button = 0; button < pad->GetButtonCount(); ++button) if (pad->IsButtonPressed(button)) return true;
        }
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
			//write only intact bindings, the corrupted one is replaced
			//by the default from the config on next start
			bool	valid = key->argv != nullptr;
			for ( int j = 1; valid && j < key->argc; j++ )
				valid = key->argv[j] != nullptr;

			if ( valid )
			{
				out->Printf( "keycmd %s %s%s", key->keyname, key->play ? "+": "", key->cmd->name );
				for ( int j = 1; j < key->argc; j++ )
					out->Printf( " %s", key->argv[j] );
				out->Printf( "\n" );
			}
		}
	}
    for (auto i = size_t(0); i < _gamepadBindings.size(); ++i) {
        const auto& key = _gamepadBindings[i];
        if (!key.configured) continue;
        out->Printf("padcmd %zu ", i);
        if (key.type == IC_NONE) out->Printf("none\n");
        else if (key.type == IC_VAR) out->Printf("%s%s\n", key.inc ? "+" : "", key.var->GetName());
        else if (key.cmd && key.argv) {
            out->Printf("%s%s", key.play ? "+" : "", key.cmd->name);
            for (auto j = 1; j < key.argc; ++j) out->Printf(" \"%s\"", key.argv[j]);
            out->Printf("\n");
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
void nsInput::LoadKeys(const char* text) {
    while (text && *text) {
        const auto length = std::strcspn(text, "\r\n");
        auto line = std::string_view(text, length);
        const auto start = line.find_first_not_of(" \t");
        if (start != std::string_view::npos) {
            line.remove_prefix(start);
            const auto token = line.substr(0, line.find_first_of(" \t"));
            if (token == "keycmd" || token == "padcmd") g_cfg->ExecLine(std::string(line).c_str());
        }
        text += length;
        while (*text == '\r' || *text == '\n') ++text;
    }
}

//---------------------------------------------------------
// nsInput::BindKeyCmd: 
//---------------------------------------------------------
bool nsInput::BindKeyCmd(KeyBinding_t* key, const cmdDesc_t* cmd, int argc, const char* argv[], bool play) {
    if (!argc || !cmd) return false;
    nsCommandArgs args{argc, static_cast<char**>(my_malloc(sizeof(char*) * argc))};
    if (!args.argv) return false;
    std::fill_n(args.argv, argc, nullptr);
    try {
        for (auto i = 0; i < argc; ++i) {
            args.argv[i] = my_strdup(argv[i]);
            if (!args.argv[i]) {
                FreeArgs(args);
                return false;
            }
        }
    } catch (...) {
        FreeArgs(args);
        throw;
    }
    UnbindKey(key);
    key->type = IC_CMD;
    key->cmd = cmd;
    key->argc = argc;
    key->argv = args.argv;
    key->play = play;
    return true;
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
void nsInput::FreeArgs(nsCommandArgs args) {
    if (!args.argv) return;
    for (auto i = 0; i < args.argc; ++i) my_free(args.argv[i]);
    my_free(args.argv);
}

void nsInput::UnbindKey(KeyBinding_t* key) {
    if (key->type == IC_CMD && key->argv) {
        if (_dispatchDepth) _retiredArgs.push_back({key->argc, key->argv});
        else FreeArgs({key->argc, key->argv});
    }
    key->type = IC_NONE;
    key->cmd = nullptr;
    key->argc = 0;
    key->argv = nullptr;
    key->play = false;
    key->inc = key->configured = false;
}

void nsInput::FinishCommand() {
    if (--_dispatchDepth == 0) {
        for (const auto& args : _retiredArgs) FreeArgs(args);
        _retiredArgs.clear();
    }
}

void nsInput::ExecuteCommand(const KeyBinding_t* key) {
    const auto command = key->cmd;
    const auto argc = key->argc;
    const auto argv = const_cast<const char**>(key->argv);
    ++_dispatchDepth;
    try { command->func(argc, argv); }
    catch (...) {
        FinishCommand();
        throw;
    }
    FinishCommand();
}

//---------------------------------------------------------
// nsInput::ShowKeys: 
//---------------------------------------------------------
void nsInput::ShowKeys()
{
	Log::Info("------------------------- key names ------------------------" );
	int	count = 0;
	for (auto & m_inKey : m_inKeys)
		if ( strlen(m_inKey.keyname) )
		{
			Log::Info("%s", m_inKey.keyname );
/*
			if ( count > 4 )
			{
				Log::Info("" );
				count = -1;
			}
*/
			count ++;
		}
	Log::Info("\n------------------------------------------------------------" );
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


	return nullptr;
}

//---------------------------------------------------------
// nsInput::KeyPressed: 
//---------------------------------------------------------
void nsInput::KeyPressed(KeyBinding_t *key)
{
	if ( key->type == IC_CMD && key->play )
		ExecuteCommand(key);
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
		_varPresses.push_back({key->var, key->inc ? 1 : -1});
		if ( key->inc )
            key->var->Inc();
		else
            key->var->Dec();
	}
	else if ( key->type == IC_CMD )
		ExecuteCommand(key);
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
    _appActive = active;
    if (!active) {
        _cursorInClient = false;
        m_bAnyKey = false;
        _varPresses.clear();
        CancelGamepads();
    }
    UpdateCursorState();
}
