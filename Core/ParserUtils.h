//------------------------------------------------------
// Coding by Gaikov Roman (2007/04/21)
// ParserUtils.h
//------------------------------------------------------
#ifndef _ParserUtils_H_
#define _ParserUtils_H_

#include "nsLib/comm_types.h"
#include "Core/Parser.h"
#include "nsLib/StrTools.h"

bool	ParseStr( script_state_t *ss, const char *name, char val[MAX_STR_LEN] );
bool	ParseName( script_state_t *ss, char val[MAX_NAME_LEN] );
const char*	ParseStrP( script_state_t *ss, const char *name, const char *def = nullptr ); //WARNING: returns the same pointer
nsString ParseString(script_state_t *ss, const char *name, const char *def = nullptr);
float	ParseFloat( script_state_t *ss, const char *name, float def = 0 );
bool	ParseColor( script_state_t *ss, const char *name, float c[4] );
bool	ParseFloat2( script_state_t *ss, const char *name, float val[2], float defVal[2] = nullptr );
bool	ParseFloat3( script_state_t *ss, const char *name, float val[3] );
bool	ParseFloat4( script_state_t *ss, const char *name, float val[4] );

#endif