#pragma once

#include <cstdint>
#include <vector>
#include "nsLib/StrTools.h"
#include "Core/Config.h"
#include "KeyCodes.h"

class IGamepad;

typedef enum
{
	IC_NONE = 0,
	IC_VAR,
	IC_CMD
}
ic_type_t;

#define MAX_KEY_NAME	32

typedef struct
{
	char		keyname[MAX_KEY_NAME];
	ic_type_t	type;
    bool configured; // Explicit padcmd entry, including none.

	union
	{
		//cmd
		struct
		{
			const cmdDesc_t	*cmd;
			int				argc;
			char			**argv;
			bool			play;		//call command continuous
		};

		//var
		struct
		{
			nsVar	*var;
			bool	inc;		//true - increment var by 1 on key down, decrement by 1 on key up... false - otherwise
		};
	};
}
KeyBinding_t;



//---------------------------------------------------------
// nsInput: 
//---------------------------------------------------------
class nsInput : public ICfgWriter
{
public:
	nsInput();
	void			Init();
	void			Release();
	void			Process();

	void			ReleaseAllKeys();
    int GetVarPressDirection(const nsVar *var) const;

	void			Unbind( const char *var_cmd );
	int				GetBind( const char *var_cmd, nsString **keys );

	void			ShowCursor( bool show );
	static void		SetCursorPos( float x, float y );
	static void		GetCursorPos( float &x, float &y );
	bool			CanDrawCursor();

	//���������� ��� ������� (�������������, ��������� � ����)
	const char*		GetKeyDesc( int code );

	bool            HasKey(const char *desc);

	bool			WaitAnyKey();
	static bool		IsAnyKeyPressed();

    void OnActivateApp(bool active);
private:
	KeyBinding_t	_keys[KEYBOARD_MAX_KEYS]{};
	bool			_previousKeys[KEYBOARD_MAX_KEYS]{};
	bool			_awaitRelease[KEYBOARD_MAX_KEYS]{};
    struct nsVarPress {
        const nsVar *var;
        int direction;
    };
    std::vector<nsVarPress> _varPresses;

	bool			_cursorVisible = true;	//cursor state (hidden/visible)
    bool            _cursorInClient = false;

    struct nsButtonState {
        bool previous = false;
        bool awaitRelease = false;
    };
    struct nsGamepadState {
        uint64_t connectionId;
        bool seen = false;
        std::vector<nsButtonState> buttons;
    };
    struct nsCommandArgs {
        int argc;
        char** argv;
    };
    std::vector<KeyBinding_t> _gamepadBindings;
    std::vector<nsGamepadState> _gamepads;
    std::vector<nsCommandArgs> _retiredArgs;
    int _dispatchDepth = 0;
    uint64_t _gamepadResetGeneration = 0;
    bool _appActive = true;

	bool			_uiActive;
	bool			_anyKey;

private:
    static void PadCmd(int argc, const char* argv[]);
    static void PrintBinding(const KeyBinding_t* binding);
	static void		keycmd_f( int argc, const char *argv[] );
	static void		in_showkeys_f( int argc, const char *argv[] );
	static void		keydesc_f( int argc, const char *argv[] );

	virtual void	OnSaveConfig( IDataWriter *out );
	void			LoadBindings();
	static void		LoadKeys(const char* text);

	bool			BindKeyCmd( KeyBinding_t *key, const cmdDesc_t *cmd, int argc, const char *argv[], bool play );
	void			BindKeyVar( KeyBinding_t *key, nsVar *var, bool inc );
	void			UnbindKey( KeyBinding_t *key );

	void			ShowKeys();

    void            UpdateCursorState();

	KeyBinding_t*		GetKey( const char *name );
	void				KeyPressed( KeyBinding_t *key );
	static void			KeyUp( KeyBinding_t *key );
	void			KeyDown( KeyBinding_t *key );

    static void FreeArgs(nsCommandArgs args);
    void ExecuteCommand(const KeyBinding_t* key);
    void FinishCommand();
    void SyncGamepads();
    nsGamepadState* FindGamepad(uint64_t connectionId);
    static const IGamepad* FindPlatformGamepad(uint64_t connectionId);
    void ReleaseGamepad(nsGamepadState& state);
    void CancelGamepads();
    void CancelGamepadBinding(int index);
    void ProcessGamepads();
    bool GamepadsBlocked() const;

	static bool			IsSystemKeyPressed();
};

extern	nsInput		g_inp;
