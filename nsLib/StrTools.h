#ifndef _STR_TOOLS_H_
#define _STR_TOOLS_H_

#include "comm_types.h"
#include "headers.h"

//---------------------------------------------------------
// str
//---------------------------------------------------------
const char*	StrPrintf( const char* fmt, ... );
bool		StrEqual( const char* s1, const char *s2 );
byte		StrHash( const char* str );
const char*	StrTabs( int tabCount );
void		StrReplace( char *str, char chFind, char chReplace );
void		StrCopy( char *str, const char *from, int maxSize = 0 );
int			StrLineCount( const char* str, int len );
const char* StrSkipChars( const char* str, const char* charSet );	//������� ������ ��������� ������� �������� ��� � ������ charSet
bool		StrCheck( const char* str );
void		StrCharReplace( char *str, char oldCh, char newCh );

//digitCount - ����� ���� (����������� ������), 89 -> (0089), ���� �������� 4
const char*	StrNumber( int num, int digitCount );

/* ���������� ��������� �� 
*/
char*		StrToken( const char *strToken, const char *strDelimit, int &resLength );

//---------------------------------------------------------
// nsString:
//---------------------------------------------------------
class nsString
{
public:
	enum { MAX_SIZE = 256 };

public:
	inline nsString();

	inline nsString( const char *str );

	inline operator const char* () const;
	inline nsString& operator = ( const char* str );
	inline bool operator == ( const char* str ) const;
	bool operator == (const nsString &str) const;
	inline bool operator != ( const char* str );
	inline char& operator [] ( int idx );
	
	void				operator += ( const char* str );
	
	void        		Format( const char *fmt, ... );
	inline char*		AsChar() { return m_str; }
	void				CopyFrom( const char* str, int length );	//���������� �����������
	void				Append( char ch );
	int					Length() const;
	void				ToLower();
    void                ToUpper();
	bool                IsEmpty() const { return Length() == 0; }

    friend nsString operator+(const nsString &s1, const char *s2);

protected:
	char	m_str[MAX_SIZE];
};

//---------------------------------------------------------
// nsString::nsString:
//---------------------------------------------------------
nsString::nsString()
{
	m_str[0] = 0;
}

//---------------------------------------------------------
// nsString::nsString:
//---------------------------------------------------------
nsString::nsString( const char *str )
{
	*this = str;
}

//---------------------------------------------------------
// nsString::operator const char*:
//---------------------------------------------------------
nsString::operator const char* () const
{
	return m_str;
}

//---------------------------------------------------------
// nsString::operator = :
//---------------------------------------------------------
nsString& nsString::operator = ( const char* str )
{
	if ( str )
	{
		strncpy( m_str, str, MAX_SIZE - 1 );
		m_str[MAX_SIZE - 1] = 0;
	}
	else
		memset( m_str, 0, MAX_SIZE );

	return *this;
}

//---------------------------------------------------------
// nsString::operator ==:
//---------------------------------------------------------
bool nsString::operator == ( const char* str ) const
{
	if ( !str ) return false;
	return strcmp(m_str, str) == 0;
}

//---------------------------------------------------------
// nsString::operator != : 
//---------------------------------------------------------
bool nsString::operator != ( const char* str )
{
	return !(*this == str);
}

//---------------------------------------------------------
// nsString::operator []: 
//---------------------------------------------------------
char& nsString::operator [] ( int idx )
{
	return m_str[idx];
}

#endif
 