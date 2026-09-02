// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file time.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "TimeFormat.h"
#include "nsLib/StrTools.h"
#include "nsLib/MathTools.h"

//---------------------------------------------------------
// TimeSecSplit: 
//---------------------------------------------------------
void Time_SecSplit( float sec, int &h, int &m, int &s )
{
	sec = (float)Math_Round( sec );
	
	h = int(sec / 3600.0f);
	m = int((sec - (h * 3600.0f)) / 60.0f);
	s = int((sec - (h * 3600.0f) - (m * 60.0f)));
}

//---------------------------------------------------------
// TimeFormat:
//---------------------------------------------------------
void Time_Format( char *desc, const std::size_t descSize, float sec, bool fillZero )
{
	if ( !desc || descSize == 0 ) return;

	int	h, m, s;
	Time_SecSplit( sec, h, m, s );
	
	if ( fillZero || h > 0 )
		snprintf( desc, descSize, "%i:%02i:%02i", h, m, s );
	else
		snprintf( desc, descSize, "%i:%02i", m, s );
}

//---------------------------------------------------------
// TimeFormat compatibility overload:
//---------------------------------------------------------
void Time_Format( char *desc, float sec, bool fillZero )
{
	Time_Format( desc, TIME_FORMAT_BUFFER_SIZE, sec, fillZero );
}

//---------------------------------------------------------
// TimeFormat:
//---------------------------------------------------------
const char* Time_Format( float sec, bool fillZero )
{
	static nsString str;
	Time_Format( str.AsChar(), nsString::MAX_SIZE, sec, fillZero );
	return str;
}
