// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Net.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_Net_H_
#define	_Net_H_

void	HTTP_GoLink( const char* link );
char*	HTTP_ConvURLParam( const char* param );	//память долюжна быть освобождена my_free

#endif //_Net_H_