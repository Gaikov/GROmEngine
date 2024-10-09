#ifndef _EDITSTRING_H_
#define _EDITSTRING_H_

#include "StrTools.h"

class nsEditString  
{
public:
	explicit nsEditString( int maxLen );

	void				SetLine( const char* line );
	inline const char*	GetLine() { return m_line; }
	inline int			GetCursorPos() { return m_cursorPos; }
	void				TypeChar( char ch );
	
	void				KeyHome();
	void				KeyEnd();
	void				KeyBackspace();
	void				KeyDelete();
	void				KeyLeft();
	void				KeyRight();

private:
	nsString	m_line;
	int			m_cursorPos;
	int			m_maxLen;
};

#endif
