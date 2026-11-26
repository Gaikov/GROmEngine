#include "Parser.h"
#include "headers.h"
#include "Memory.h"
#include "nsLib/EasyStack.h"
#include "nsLib/StrTools.h"

#define		STR_DELIMIT		" \t\r\n"

namespace {
    class nsScriptBlockScanner final {
    public:
        static char *FindSymbol(char *text, const char symbol) {
            if (!text) return nullptr;
            auto quoted = false;
            for (auto cursor = text; *cursor; ++cursor) {
                if (*cursor == '"') quoted = !quoted;
                else if (!quoted && *cursor == symbol) return cursor;
            }
            return nullptr;
        }

        static char *FindClosingBrace(char *opening) {
            if (!opening || *opening != '{') return nullptr;
            auto depth = 1;
            auto quoted = false;
            for (auto cursor = opening + 1; *cursor; ++cursor) {
                if (*cursor == '"') quoted = !quoted;
                else if (!quoted) {
                    if (*cursor == '{') ++depth;
                    else if (*cursor == '}' && --depth == 0) return cursor;
                }
            }
            return nullptr;
        }
    };
}

//---------------------------------------------------------
// block_state_t:
//---------------------------------------------------------
struct block_state_t
{
	char	*block;						//block data (text)
	char	name[PS_MAX_NAME_LEN];		//block name
	char	*b_ptr;						//block ptr in prev block
	bool	any;						//any block must fetch
};

//---------------------------------------------------------
// script_state_t:
//---------------------------------------------------------
struct script_state_t
{
	char	*global;
	nsEasyStack<block_state_t, MAX_BLOCKS>	s;
	block_state_t	bs;

	char	*var_ptr;					//current variable ptr in block
	char	var_name[PS_MAX_NAME_LEN];	//var for parsing
	char	pfx;						//prefix char for scripting
};

//---------------------------------------------------------
// strconv:
//---------------------------------------------------------
void strconv( char *str )
{
	char	*tok;
	while ( (tok = strchr( str, '$' )) )
	{
		char *end = tok + 1;
		int	count = 0;
		while ( *end && isdigit( *end ) && count < 3 )
		{
			count ++;
			end++;
		}
		
		if ( count )
		{
			char	ch = tok[count + 1];
			tok[count + 1] = 0;
			*tok = atoi( tok + 1 );
			tok[count + 1] = ch;
			
			memmove( tok + 1, end, strlen( end ) + 1 );
		}
		else
			memmove( tok, end, strlen( end ) + 1 );
	}
}

//---------------------------------------------------------
// ps_delete_comments:
//---------------------------------------------------------
void ps_delete_comments(char *block) {
    auto output = block;
    auto quoted = false;
    while (*block) {
        if (!quoted && block[0] == '/' && block[1] == '*') {
            // Comment contents are literal: glyph labels may contain quotes or braces.
            const auto end = strstr(block + 2, "*/");
            if (end) {
                block = end + 2;
                continue;
            }
            // Preserve an unterminated block comment, as the legacy parser did.
            memmove(output, block, strlen(block) + 1);
            return;
        }
        if (!quoted && (*block == ';' || (block[0] == '/' && block[1] == '/'))) {
            while (*block && *block != '\n') ++block;
            continue;
        }
        if (*block == '"') quoted = !quoted;
        *output++ = *block++;
    }
    *output = 0;
}

//---------------------------------------------------------
// ps_begin:
//---------------------------------------------------------
script_state_t*	ps_begin( char *block, char prefix )
{
	if ( !block || !strlen(block) ) return nullptr;
	
	auto	*ss = (script_state_t*)my_malloc( sizeof(script_state_t) );
	if ( !ss ) return nullptr;
	memset( ss, 0, sizeof(script_state_t) );

	ps_delete_comments( block );

	ss->global = block;
	ss->pfx = prefix;
	return ss;
}

//---------------------------------------------------------
// ps_end:
//---------------------------------------------------------
void	ps_end( script_state_t *ss )
{
	block_state_t	b = ss->bs;
	do
	{
		my_free( b.block );
	}
	while ( ss->s.Pop( b ) );


	my_free( ss );
}

//---------------------------------------------------------
// ps_str_equal:
//---------------------------------------------------------
bool	ps_str_equal( char *block, const char *name, char pfx )
{
	int		len = 0;
	char	*var = StrToken( block, STR_DELIMIT, len );
	if ( !var ) return false;

	int	nlen = strlen( name ) + 1;
	len = nlen > len ? nlen : len;

	return (strncmp( var, StrPrintf( "%c%s", pfx, name ), len ) == 0);
}


//---------------------------------------------------------
// ps_is_block:
//---------------------------------------------------------
bool	ps_is_block( char *text, char pfx )
{
	int	len = 0;
	if ( !text ) return false;
	
	char	*token = nsScriptBlockScanner::FindSymbol(text, pfx);	if ( !token ) return false;
	if ( !StrToken( token, STR_DELIMIT, len ) ) return false;

	token = StrToken( nullptr, STR_DELIMIT, len );

	return token && token[0] == '{';
}

//---------------------------------------------------------
// ps_skip_block:
//---------------------------------------------------------
char* ps_skip_block( char *text ) {
    const auto opening = nsScriptBlockScanner::FindSymbol(text, '{');
    const auto closing = nsScriptBlockScanner::FindClosingBrace(opening);
    return closing ? closing + 1 : nullptr;
}

//---------------------------------------------------------
// ps_find_block:
//---------------------------------------------------------
char* ps_find_block( char *from, const char *name, char pfx ) //if name == NULL search any block
{
	char	*token = from;
	while ( token && (token = nsScriptBlockScanner::FindSymbol(token, pfx)) )
	{
		if ( !name || !strlen( name ) )
		{
			if ( ps_is_block( token, pfx ) )
				return token;
		}
		else if ( ps_is_block( token, pfx ) )
		{
			if ( ps_str_equal( token, name, pfx ) ) return token;
			else token = ps_skip_block( token );
		}

		if ( token ) token ++;
	}

	return nullptr;
}

//---------------------------------------------------------
// ps_block_len:
//---------------------------------------------------------
char* ps_block_len( char *from, /*out*/ int &len ) {
    len = 0;
    const auto opening = nsScriptBlockScanner::FindSymbol(from, '{');
    const auto closing = nsScriptBlockScanner::FindClosingBrace(opening);
    if (!closing) return nullptr;
    const auto begin = opening + 1;
    len = static_cast<int>(closing - begin);
    return begin;
}

//---------------------------------------------------------
// ps_var_name:
//---------------------------------------------------------
bool ps_var_name( char *from, char pfx, /*out*/char name[PS_MAX_NAME_LEN] )
{
	name[0] = 0;
	if ( !from ) return false;
		
	char *token = nsScriptBlockScanner::FindSymbol(from, pfx);
	if ( !token ) return false;
	token ++;

	int	len;
	token = StrToken( token, STR_DELIMIT, len );
	if ( !token ) return false;

	memset( name, 0, PS_MAX_NAME_LEN );
	len = (len >= PS_MAX_NAME_LEN) ? PS_MAX_NAME_LEN - 1 : len;
	strncpy( name, token, len );

	return true;
}

//---------------------------------------------------------
// ps_alloc_block:
//---------------------------------------------------------
char* ps_alloc_block( char *from, /*out*/ char name[PS_MAX_NAME_LEN], char pfx )
{
	name[0] = 0;
	int	len = 0;
	
	char	*token = from;
	while ( !ps_is_block( token, pfx ) )
		token = ps_skip_block( token );

	if ( ps_var_name( token, pfx, name ) )
	{
		char *block = ps_block_len( token, len );
		if ( !block ) return nullptr;

		char  *res = (char*)my_malloc( len + 1 );
		if ( !res ) return nullptr;
		memset( res, 0, len + 1 );

		strncpy( res, block, len );
		return res;
	}
	return nullptr;
}

//---------------------------------------------------------
// ps_block_begin:
//---------------------------------------------------------
bool ps_block_begin( script_state_t *ss, const char *name )
{
	if ( !ss ) return false;
	char	*from = ss->bs.block ? ss->bs.block : ss->global;
	char	*start = ps_find_block( from, name, ss->pfx );
	if ( !start ) return false;

	char	b_name[PS_MAX_NAME_LEN];
	char	*block = ps_alloc_block( start, b_name, ss->pfx );
	if ( !block ) return false;

	if ( !ss->s.Push( ss->bs ) )
	{
		my_free( block );
		return false;	
	}
	
	memset( &ss->bs, 0, sizeof(block_state_t) );
	ss->bs.b_ptr = start;
	ss->bs.block = block;
	ss->bs.any = name ? false : true;
	strcpy( ss->bs.name, b_name );

	ss->var_ptr = nullptr;

	return true;
}

//---------------------------------------------------------
// ps_block_next:
//---------------------------------------------------------
bool ps_block_next( script_state_t *ss )
{
	if ( !ss || !ss->global ) return false;
	if ( !ss->bs.block || !ss->bs.b_ptr ) return false;

	char	*from = ss->bs.b_ptr;
	bool	any = ss->bs.any;
	
	if ( !(from = ps_skip_block( from )) ) return false;
	char	*start = ps_find_block( from, any ? nullptr : ss->bs.name, ss->pfx );
	if ( !start ) return false;

	char	b_name[PS_MAX_NAME_LEN];
	char	*block = ps_alloc_block( start, b_name, ss->pfx );
	if ( !block ) return false;

	my_free( ss->bs.block );
	memset( &ss->bs, 0, sizeof(block_state_t) );
	ss->bs.b_ptr = start;
	ss->bs.block = block;
	ss->bs.any = any;
	strcpy( ss->bs.name, b_name );

	ss->var_ptr = nullptr;

	return true;
}

//---------------------------------------------------------
// ps_block_end:
//---------------------------------------------------------
void ps_block_end( script_state_t *ss )
{
	if ( !ss ) return;
	
	block_state_t	bs{};
	if ( ss->s.Pop( bs ) )
	{
		my_free( ss->bs.block );
		ss->bs = bs;
	}

	ss->var_ptr = nullptr;
}

//---------------------------------------------------------
// ps_block_get:
//---------------------------------------------------------
const char*	ps_block_get( script_state_t *ss, /*out*/ char *name, int nameSize )
{
	if ( !ss || !ss->bs.block ) return nullptr;
	strncpy( name, ss->bs.name, nameSize );
	name[nameSize-1] = 0;
	return ss->bs.block;
}

const char*     ps_block_name( script_state_t *ss) {
    if (ss && ss->bs.block) {
        return ss->bs.name;
    }
    return nullptr;
}

//---------------------------------------------------------
// ps_var_search:
//---------------------------------------------------------
char* ps_var_search( char *from, const char *name, char pfx )
{
	while ( (from = nsScriptBlockScanner::FindSymbol(from, pfx)) )
	{
		if ( ps_is_block( from, pfx ) )
		{
			from = ps_skip_block( from );
			if ( !from ) return nullptr;
		}
		else if ( ps_str_equal( from, name, pfx ) )
			return from;
		else
			from ++;
	}

	return nullptr;
}

//---------------------------------------------------------
// ps_var_begin:
//---------------------------------------------------------
bool ps_var_begin( script_state_t *ss, const char *name )
{
	if ( !ss || !ss->global || !name || !strlen( name ) ) return false;
	char	*from = ss->bs.block ? ss->bs.block : ss->global;
	
	ss->var_ptr = ps_var_search( from, name, ss->pfx );
	
	memset( ss->var_name, 0, PS_MAX_NAME_LEN );
	strncpy( ss->var_name, name, PS_MAX_NAME_LEN - 1 );
	return ss->var_ptr != nullptr;
}

//---------------------------------------------------------
// ps_var_next:
//---------------------------------------------------------
bool ps_var_next( script_state_t *ss )
{
	if ( !ss->var_ptr || !strlen( ss->var_name ) ) return false;
	ss->var_ptr = ps_var_search( ss->var_ptr + 1, ss->var_name, ss->pfx );
	return ss->var_ptr != nullptr;
}

//---------------------------------------------------------
// ps_var_f:
//---------------------------------------------------------
float ps_var_f( script_state_t *ss )
{
	if ( !ss->var_ptr ) return 0;

	const char	*fmt = StrPrintf( "%c%s %s", ss->pfx, ss->var_name, "%f" );
	float	value = 0;
	sscanf( ss->var_ptr, fmt, &value );

	return value;
}

//---------------------------------------------------------
// ps_var_2f:
//---------------------------------------------------------
bool ps_var_2f( script_state_t *ss, float val[2] )
{
	if ( !ss->var_ptr ) return false;

	const char	*fmt = StrPrintf( "%c%s %s", ss->pfx, ss->var_name, "%f %f" );
	sscanf( ss->var_ptr, fmt, &val[0], &val[1] );

	return true;
}

//---------------------------------------------------------
// ps_var_3f:
//---------------------------------------------------------
bool ps_var_3f( script_state_t *ss, float val[3] )
{
	if ( !ss->var_ptr ) return false;

	const char	*fmt = StrPrintf( "%c%s %s", ss->pfx, ss->var_name, "%f %f %f" );
	sscanf( ss->var_ptr, fmt, &val[0], &val[1], &val[2] );

	return true;
}

//---------------------------------------------------------
// ps_var_4f:
//---------------------------------------------------------
bool ps_var_4f( script_state_t *ss, float val[4] )
{
	if ( !ss->var_ptr ) return false;

	const char	*fmt = StrPrintf( "%c%s %s", ss->pfx, ss->var_name, "%f %f %f %f" );
	sscanf( ss->var_ptr, fmt, &val[0], &val[1], &val[2], &val[3] );

	return true;
}

//---------------------------------------------------------
// ps_var_str:
//---------------------------------------------------------
bool ps_var_str( script_state_t *ss, char *str, int len )
{
	if ( !ss->var_ptr || !str ) return false;

	int	tok_len;
	if ( !StrToken( ss->var_ptr, STR_DELIMIT, tok_len ) ) return false;

	char	*start = StrToken( nullptr, STR_DELIMIT, tok_len );
	if ( !start || start[0] != '"' ) return false;
	char	*end = strchr( start + 1, '"' );	if ( !end ) return false;

	start ++;
	int	real_len = end - start;
	len = real_len >= len ? len - 1 : real_len;
	
	strncpy( str, start, len );
	str[len] = 0;

	strconv( str );
	return true;
}

//---------------------------------------------------------
// ps_var_str:
//---------------------------------------------------------
char* ps_var_str( script_state_t *ss )
{
	if ( !ss->var_ptr ) return nullptr;

	static	char	str[PS_MAX_STR_LEN];
	int	tok_len;
	if ( !StrToken( ss->var_ptr, STR_DELIMIT, tok_len ) ) return nullptr;

	char	*start = StrToken( nullptr, STR_DELIMIT, tok_len );
	if ( !start || start[0] != '"' ) return nullptr;
	char	*end = strchr( start + 1, '"' );	if ( !end ) return nullptr;

	start ++;
	int	real_len = end - start;
	int	len = real_len >= PS_MAX_STR_LEN ? PS_MAX_STR_LEN - 1 : real_len;
	
	strncpy( str, start, len );
	str[len] = 0;

	strconv( str );
	return str;
}

//---------------------------------------------------------
// ps_var_fmt:
//---------------------------------------------------------
const char* ps_var_fmt( script_state_t *ss, const char *fmt )
{
	if ( !ss->var_ptr ) return nullptr;
	return StrPrintf( "%c%s %s", ss->pfx, ss->var_name, fmt );	
}

//---------------------------------------------------------
// ps_var_ptr:
//---------------------------------------------------------
const char* ps_var_ptr( script_state_t *ss )
{
	return ss->var_ptr;
}


//---------------------------------------------------------
// ps_test:
//---------------------------------------------------------
void ps_test( char *data )
{
	/*char	name[PS_MAX_NAME_LEN];
	int		len;
	char	*block = ps_find_block( data, 0 );
	block = ps_alloc_block( block, name, len );//*/

	/*char	name2[PS_MAX_NAME_LEN];
	int		len2;
	char	*block2 = ps_find_block( data, "block5" );
	block2 = ps_alloc_block( block2, name2, len2 );//*/

	/*char	var[100][64];
	int		i = 0;
	script_state_t	*ss = ps_begin( data, '$' );
	if ( !ps_block_begin( ss, "main" ) ) return;
	if ( !ps_block_begin( ss, "block" ) ) return;
	
	do
	{
		if ( !ps_var_begin( ss, "var" ) ) return;
		
		do
		{
			if ( !ps_var_str( ss, var[i], 64 ) ) break;
			i ++;
		}
		while ( ps_var_next( ss ) );
	}
	while ( ps_block_next( ss ) );

	ps_end( ss );//*/

}

//---------------------------------------------------------
// ps_get_f:
//---------------------------------------------------------
float ps_get_f( script_state_t *ss, const char *name, float defVal )
{
	if ( !ss || !name || !ps_var_begin( ss, name ) ) return defVal;
	return ps_var_f( ss );
}

//---------------------------------------------------------
// ps_get_2f:
//---------------------------------------------------------
bool ps_get_2f( script_state_t *ss, const char *name, float val[2], const float defVal[2] )
{
	if ( !ss || !name 
		|| !ps_var_begin( ss, name )
		|| !ps_var_2f( ss, val ) )
	{
		if ( defVal )
		{
			val[0] = defVal[0];
			val[1] = defVal[1];
		}
		else
			val[0] = val[1] = 0;
		return false;
	}
	return true;
}

//---------------------------------------------------------
// ps_get_3f:
//---------------------------------------------------------
bool ps_get_3f( script_state_t *ss, const char *name, float val[3], const float defVal[3] )
{
	if ( !ss || !name 
		|| !ps_var_begin( ss, name )
		|| !ps_var_3f( ss, val ) )
	{
		if ( defVal )
		{
			val[0] = defVal[0];
			val[1] = defVal[1];
			val[2] = defVal[2];
		}
		else
			val[0] = val[1] = val[2] = 0;
		return false;
	}
	return true;
}

//---------------------------------------------------------
// ps_get_4f:
//---------------------------------------------------------
bool ps_get_4f( script_state_t *ss, const char *name, float val[4], const float defVal[4] )
{
	if ( !ss || !name 
		|| !ps_var_begin( ss, name )
		|| !ps_var_4f( ss, val ) )
	{
		if ( defVal )
		{
			val[0] = defVal[0];
			val[1] = defVal[1];
			val[2] = defVal[2];
			val[3] = defVal[3];
		}
		else
			val[0] = val[1] = val[2] = val[3] = 0;
		return false;
	}
	return true;
}

//---------------------------------------------------------
// ps_get_color: 
//---------------------------------------------------------
bool ps_get_color( script_state_t *ss, const char *name, float val[4] )
{
	float	defColor[] = { 1, 1, 1, 1 };
	return ps_get_4f( ss, name, val, defColor );
}

//---------------------------------------------------------
// ps_get_str:
//---------------------------------------------------------
const char* ps_get_str( script_state_t *ss, const char *name, const char *defVal )
{
	if ( !ss || !name || !ps_var_begin( ss, name ) ) return defVal;
	const char *str = ps_var_str( ss );
	if ( !str ) return defVal;
	return str;
}
