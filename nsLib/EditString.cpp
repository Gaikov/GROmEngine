#include "EditString.h"

//---------------------------------------------------------
// nsEditString::nsEditString:
//---------------------------------------------------------
nsEditString::nsEditString( int maxLen )
{
	m_maxLen = maxLen <= 0 || maxLen > nsString::MAX_SIZE - 1 ? nsString::MAX_SIZE - 1 : maxLen;
	m_cursorPos = 0;
}

//---------------------------------------------------------
// nsEditString::SetLine:
//---------------------------------------------------------
void nsEditString::SetLine( const char* line )
{
	m_line = line;
	m_cursorPos = line ? strlen( line ) : 0;
}

//---------------------------------------------------------
// nsEditString::TypeChar:
//---------------------------------------------------------
void nsEditString::TypeChar( char ch )
{
	int	len = strlen( m_line );
	if ( len >= m_maxLen ) return;
	
	if ( m_cursorPos < len )
	{
		char	*strLine = m_line.AsChar();
		memmove( &strLine[m_cursorPos+1], &strLine[m_cursorPos], len - m_cursorPos + 1 );
		strLine[m_cursorPos] = ch;
	}
	else
		m_line.Append( ch );
	m_cursorPos ++;
}

//---------------------------------------------------------
// nsEditString::KeyHome:
//---------------------------------------------------------	
void nsEditString::KeyHome()
{
	m_cursorPos = 0;
}

//---------------------------------------------------------
// nsEditString::KeyEnd:
//---------------------------------------------------------
void nsEditString::KeyEnd()
{
	m_cursorPos = strlen( m_line );
}

//---------------------------------------------------------
// nsEditString::KeyBackspace:
//---------------------------------------------------------
void nsEditString::KeyBackspace()
{
	if ( !m_cursorPos ) return;
	
	int		len = strlen( m_line );
	char	*strLine = m_line.AsChar();
	if ( m_cursorPos >= len )
	{
		m_cursorPos --;
		strLine[m_cursorPos] = 0;
	}
	else
	{
		memmove( &strLine[m_cursorPos-1], &strLine[m_cursorPos], len - m_cursorPos + 1 );
		m_cursorPos --;
	}
}

//---------------------------------------------------------
// nsEditString::KeyDelete:
//---------------------------------------------------------
void nsEditString::KeyDelete()
{
	int	len = strlen( m_line );
	if ( m_cursorPos < len )
	{
		char	*strLine = m_line.AsChar();
		memmove( &strLine[m_cursorPos], &strLine[m_cursorPos+1], len - m_cursorPos );
	}
}

//---------------------------------------------------------
// nsEditString::KeyLeft:
//---------------------------------------------------------
void nsEditString::KeyLeft()
{
	m_cursorPos --;
	if ( m_cursorPos < 0 ) m_cursorPos = 0;
}

//---------------------------------------------------------
// nsEditString::KeyRight:
//---------------------------------------------------------
void nsEditString::KeyRight()
{
	int	len = strlen( m_line );
	m_cursorPos ++;
	if ( m_cursorPos > len ) m_cursorPos = len;
}

