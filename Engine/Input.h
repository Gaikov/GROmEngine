#ifndef _INPUT_H_
#define _INPUT_H_

#include "nsLib/StrTools.h"
#include "Core/Config.h"
#include "KeyCodes.h"

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
	KeyBinding_t	m_inKeys[KEYBOARD_MAX_KEYS]{};
	bool			m_kbPrev[KEYBOARD_MAX_KEYS]{};

	bool			_cursorVisible = true;	//cursor state (hidden/visible)
    bool            _cursorInClient = false;

	KeyBinding_t	m_joyKeys[8]{};
	bool			m_joyPrev[8]{};
	bool			m_joyUse;

	bool			m_uiActive;
	bool			m_bAnyKey;

private:
	static void		keycmd_f( int argc, const char *argv[] );
	static void		in_showkeys_f( int argc, const char *argv[] );
	static void		keydesc_f( int argc, const char *argv[] );

	virtual void	OnSaveConfig( IDataWriter *out );
	void			LoadBindings();
	static void		LoadKeys( char *str );

	void			BindKeyCmd( KeyBinding_t *key, const cmdDesc_t *cmd, int argc, const char *argv[], bool play );
	void			BindKeyVar( KeyBinding_t *key, nsVar *var, bool inc );
	static void		UnbindKey( KeyBinding_t *key );

	void			ShowKeys();

    void            UpdateCursorState();

	KeyBinding_t*		GetKey( const char *name );
	static void			KeyPressed( KeyBinding_t *key );
	static void			KeyUp( KeyBinding_t *key );
	static void			KeyDown( KeyBinding_t *key );

	static void			GetJoyState( bool keys[8] );

	static bool			IsSystemKeyPressed();
};

extern	nsInput		g_inp;

#endif
