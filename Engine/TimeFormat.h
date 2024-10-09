// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file time.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_TimeFormat_H_
#define	_TimeFormat_H_

void		Time_SecSplit( float sec, int &h, int &m, int &s );
void		Time_Format( char *desc, float sec, bool fillZero = true );
const char* Time_Format( float sec, bool fillZero = true );

extern float g_frameTime;

#endif //_TimeFormat_H_