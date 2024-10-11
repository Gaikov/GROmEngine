#include "StrTools.h"

//---------------------------------------------------------
// StrPrintf:
//---------------------------------------------------------
const char* StrPrintf( const char* fmt, ... )
{
	static char str[1024];

	va_list	list;
	va_start( list, fmt );
	vsprintf( str, fmt, list );
	va_end( list );

	return str;
}

//---------------------------------------------------------
// StrEqual:
//---------------------------------------------------------
bool StrEqual( const char* s1, const char *s2 )
{
	while ( *s1 && *s2 )
	{
		if ( *s1 != *s2 ) return false;

		s1 ++;
		s2 ++;
	}
	return *s1 == *s2;
}

//---------------------------------------------------------
// StrHash:
//---------------------------------------------------------
byte StrHash( const char* str )
{
	uint	hash = 0;
	while ( *str )
	{
		hash += *str;
		str++;
	}
	return byte(hash % 255);
}

//---------------------------------------------------------
// StrTabs:
//---------------------------------------------------------
const char* StrTabs( int tabCount )
{
	static char	res[256];
    if ( tabCount >= 256 ) tabCount = 255;
    memset( res, '\t', tabCount );
    res[tabCount] = 0;
    return res;
}

//---------------------------------------------------------
// StrReplace:
//---------------------------------------------------------
void StrReplace( char *str, char chFind, char chReplace )
{
	while ( *str )
    {
    	if ( *str == chFind )
        	*str = chReplace;
    	str++;
    }
}

//---------------------------------------------------------
// StrCopy: 
//---------------------------------------------------------
void StrCopy( char *str, const char *from, int maxSize )
{
	if ( !str || !from ) return;

	int	len = strlen( from );
	if ( maxSize > 0 )
		len = len > maxSize - 1 ? maxSize - 1 : len;

	strncpy( str, from, len );
	str[len] = 0;
}

//---------------------------------------------------------
// StrToken:
//---------------------------------------------------------
char *StrToken( const char *strToken, const char *strDelimit, int &resLength )
{
	static	const char	*end = 0;

	if ( !strToken ) strToken = end;
	if ( !strToken ) return 0;

	const char	*begin = strpbrk( strToken, strDelimit );
	while ( begin && begin == strToken )
	{
		strToken ++;
		if ( !*strToken )
		{
			end = 0;
			return 0;
		}
		begin = strpbrk( strToken, strDelimit );
	}

	if ( begin )
	{
		resLength = begin - strToken;
		end = begin;
		return (char*)strToken;
	}

    resLength = strlen( strToken );
	end = 0;
	return (char*)strToken;
}

//---------------------------------------------------------
// StrLineCount:
//---------------------------------------------------------
int StrLineCount( const char* str, int len )
{
	int	count = 0;
	for ( int i = 0; i < len; i++, str++)
		if ( *str == '\n' )	count ++;

	return count;
}

//---------------------------------------------------------
// StrSkipChars:
//---------------------------------------------------------
const char* StrSkipChars( const char* str, const char* charSet )
{
	while ( *str )
	{
		if ( !strchr( charSet, *str ) )
			break;
		str ++;
	}
	return str;
}

//---------------------------------------------------------
// StrCheck: 
//---------------------------------------------------------
bool StrCheck( const char* str )
{
	return str && strlen( str );
}

//---------------------------------------------------------
// StrCharReplace: 
//---------------------------------------------------------
void StrCharReplace( char *str, char oldCh, char newCh )
{
	while ( *str )
	{
		if ( *str == oldCh )
			*str = newCh;
		str ++;
	}
}

//---------------------------------------------------------
// StrNumber: 
//---------------------------------------------------------
const char*	StrNumber( int num, int digitCount )
{
	static nsString	out;
	nsString		fmt;
	fmt.Format( "%%0%ii", digitCount );
	out.Format( fmt, num );

	return out;
}

//---------------------------------------------------------
// nsString::operator += : 
//---------------------------------------------------------
void nsString::operator += ( const char* str )
{
	strcat( m_str, str );
}

//---------------------------------------------------------
// nsString::Format:
//---------------------------------------------------------
void nsString::Format( const char *fmt, ... )
{
	va_list	list;
	va_start( list, fmt );
	vsprintf( m_str, fmt, list );
	va_end( list );
}

//---------------------------------------------------------
// nsString::CopyFrom:
//---------------------------------------------------------
void nsString::CopyFrom( const char* str, int length )
{
	if ( !str ) return;
	if ( length <= 0 ) length = strlen( str );
	length = length > MAX_SIZE - 1 ? MAX_SIZE - 1 : length;
    strncpy( m_str, str, length );
	m_str[length] = 0;
}

//---------------------------------------------------------
// nsString::Append:
//---------------------------------------------------------
void nsString::Append( char ch )
{
	int	len = strlen( m_str );
	if ( len < MAX_SIZE - 1 )
	{
		m_str[len] = ch;
		m_str[len + 1] = 0;
	}
}

//---------------------------------------------------------
// nsString::Length: 
//---------------------------------------------------------
int	nsString::Length() const
{
	return strlen( m_str );
}

//---------------------------------------------------------
// nsString::ToLower: 
//---------------------------------------------------------
void nsString::ToLower()
{
	std::string	str = m_str;
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	strcpy(m_str, str.c_str());
}

void nsString::ToUpper() {
    std::string	str = m_str;
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    strcpy(m_str, str.c_str());
}

//---------------------------------------------------------
// operator +: 
//---------------------------------------------------------
nsString operator + ( const nsString &s1, const char *s2 )
{
	nsString	res = s1;
	res += s2;
	return res;
}

bool nsString::operator == (const nsString &str) const
{
	return *this == (const char*)str;
}

nsString nsString::Replace(const char *find, const char *replace) {
    nsString    res = *this;

    int replaceLen = int(strlen(replace));
    int findLen  = int(strlen(find));
    int moveOffset = int(replaceLen - findLen);

    while (auto found = strstr(res.m_str, find)) {
        memmove(found + + findLen + moveOffset, found + findLen, strlen(found) + 1);
        if (replaceLen > 0) {
            strcpy(found, replace);
        }
    }
    return res;
}