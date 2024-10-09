// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParserTools.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_ParserTools_H_
#define	_ParserTools_H_

#include "nsLib/BBox2.h"
#include "Core/Parser.h"

bool PT_Box( script_state_t *ss, nsBBox2 &box, const char *blockName = "bbox" );

#endif //_ParserTools_H_