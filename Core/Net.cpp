// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Net.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "Net.h"
#include "headers.h"
#include "nsLib/StrTools.h"
#include "Memory.h"

//---------------------------------------------------------
// HTTP_GoLink: 
//---------------------------------------------------------
void HTTP_GoLink( const char* link )
{
	if ( !link || !strlen( link ) ) return;
	//ShellExecute( 0, "open", link, 0, "", SW_SHOWDEFAULT );
	assert(!"not implemented");
}

//---------------------------------------------------------
// HTTP_ConvUrlParam: 
//---------------------------------------------------------
char*	HTTP_ConvURLParam( const char* param )
{
	int			len = strlen( param );
	int			outLen = len * 3;
	
	char		*res = (char*)my_malloc( outLen + 1 );
	res[outLen] = 0;

	char		p = '%';
	char		*outChar = res;

	for ( int i = 0; i < len; i++ )
	{
		unsigned int	ch = (unsigned char)param[i];
		sprintf( outChar, "%c%02X", p, ch );
		outChar += 3;
	}

	*outChar = 0;
	return res;
}