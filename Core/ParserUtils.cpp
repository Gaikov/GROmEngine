//------------------------------------------------------
// Coding by Gaikov Roman 2007/04/21
// ParserUtils.cpp
//------------------------------------------------------
#include "ParserUtils.h"
#include "nsLib/headers.h"

//------------------------------------------------------
// ParseStr: 
//------------------------------------------------------
bool ParseStr( script_state_t *ss, const char *name, char val[MAX_STR_LEN] )
{
	val[0] = 0;
	if ( !ps_var_begin( ss, name ) ) return false;
	if ( !ps_var_str( ss, val, MAX_STR_LEN ) ) return false;
	return strlen(val) != 0;
}

//------------------------------------------------------
// ParseName: 
//------------------------------------------------------
bool ParseName( script_state_t *ss, char val[MAX_NAME_LEN] )
{
	val[0] = 0;
	if ( !ps_var_begin( ss, "name" ) ) return false;
	if ( !ps_var_str( ss, val, MAX_NAME_LEN ) ) return false;
	if ( !strlen( val ) ) return false;
	return true;
}

//------------------------------------------------------
// ParseStrP: 
//------------------------------------------------------
const char* ParseStrP( script_state_t *ss, const char *name, const char *def )
{
	char *tmp;
	if ( !ps_var_begin( ss, name ) ) return def;
	tmp = ps_var_str( ss );
	return tmp ? tmp : def;
}

nsString ParseString(script_state_t *ss, const char *name, const char *def) {
    auto str = ParseStrP(ss, name);
    if (!str) {
        str = def;
    }

    nsString res = str;
    return res;
}

//------------------------------------------------------
// ParseFloat: 
//------------------------------------------------------
float ParseFloat( script_state_t *ss, const char *name, float def )
{
	if ( !ps_var_begin( ss, name ) ) return def;
	return ps_var_f( ss );
}

//------------------------------------------------------
// ParseFloat2: 
//------------------------------------------------------
bool ParseFloat2( script_state_t *ss, const char *name, float val[2], float defVal[2] )
{
	if ( !ps_var_begin( ss, name ) || !ps_var_2f( ss, val ) )
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

//------------------------------------------------------
// ParseFloat3: 
//------------------------------------------------------
bool ParseFloat3( script_state_t *ss, const char *name, float val[3] )
{
	if ( !ps_var_begin( ss, name ) || !ps_var_3f( ss, val ) )
	{
		val[0] = val[1] = val[2] = 0;
		return false;
	}
	return true;
}

//------------------------------------------------------
// ParseFloat4: 
//------------------------------------------------------
bool ParseFloat4( script_state_t *ss, const char *name, float val[4] )
{
	if ( !ps_var_begin( ss, name ) || !ps_var_4f( ss, val ) )
	{
		val[0] = val[1] = val[2] = val[3] = 0;
		return false;
	}
	return true;
}

//------------------------------------------------------
// ParseColor: 
//------------------------------------------------------
bool ParseColor( script_state_t *ss, const char *name, float c[4] )
{
	if ( !ps_var_begin( ss, name ) || !ps_var_4f( ss, c ) )
	{
		c[0] = c[1] = c[2] = c[3] = 1;
		return false;
	}
	return true;
}

bool ParseBool(script_state_t *ss, const char *name, bool defValue) {
    return ParseFloat(ss, name, defValue ? 1 : 0) > 0;
}
