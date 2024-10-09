// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParserTools.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "ParserTools.h"

//---------------------------------------------------------
// PT_Box: 
//---------------------------------------------------------
bool PT_Box( script_state_t *ss, nsBBox2 &box, const char *blockName )
{
	if ( blockName )
	{
		if ( !ps_block_begin( ss, blockName ) )
			return false;
	}

	bool	res = true;

	res &= ps_get_2f( ss, "min", box.m_min );
	res &= ps_get_2f( ss, "max", box.m_max );

	if ( blockName )
		ps_block_end( ss );

	return res;
}