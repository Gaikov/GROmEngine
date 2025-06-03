// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Console.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_Console_H_
#define	_Console_H_

#include "nsLib/StrTools.h"
#include "nsLib/CycleList.h"
#include "nsLib/color.h"
#include "nsLib/EditString.h"
#include "nsLib/log.h"
#include "UserInput.h"
#include "RenDevice.h"

class nsConsole : public IUserInput, public ILogListener, public nsSubSystem<nsConsole>
{
public:
	nsConsole();
	~nsConsole() override;

	void			Activate( bool active );
	void			Toggle();
	bool			IsActive();
	bool			IsEnabled();
	void			Clear();
	void			Draw();

protected:
	enum { CON_TEXT_SIZE = 32768 };

protected:
	char			m_text[CON_TEXT_SIZE];
	nsEditString	m_line;
	int				m_nLineUp;
	char			*m_pPrint;

	ITexture		*m_tex;
	IRenState		*_renState;
	float			tex_offs[2];

	bool						m_bActive;

	nsCycleList<nsString, 10>	m_hyst;
	std::mutex					_logMutex;

protected:
	void			DrawCursor( float y );

	char*			SkipLines( char *from, int count );
	void			ClearLine();

	char*			LineFromEnd( char *end );
	void			DrawLine( const char *line, float y, int len, const nsColor &c );

	void	        OnKeyUp( int key ) override {}
	void	        OnKeyDown( int key, bool rept ) override;
	bool            OnInit() override;
	void            OnRelease() override;

public:
    void OnMouseWheel(float delta) override;

protected:
    void	OnChar( char ch ) override ;

	bool OnPointerUp(float x, float y, int pointerId) override { return true; }
	bool OnPointerDown(float x, float y, int pointerId) override { return true; }
	bool OnPointerMove(float x, float y, int pointerId) override { return true; }
    void OnPointerCancel(int pointerId) override;

private:
	void    LogPrint(LogLevel level, const char *str) override;
    static  void clear_f(int argc, const char *argv[]);
	void	ClearUnsafe();
};


#endif //_Console_H_
