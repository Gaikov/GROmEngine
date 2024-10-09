// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file TexFrames.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_TexFrames_H_
#define	_TexFrames_H_

#include "RenDevice.h"
#include "Core/Parser.h"

class nsTexFrames
{
public:
	virtual ~nsTexFrames();

	static nsTexFrames*	LoadFromEnum( const char *filesName, const char *ext, int digitCount, int startIndex );
	static nsTexFrames* ParseFromEnum( script_state_t *ss );

	ITexture*			GetFrame( int idx );
	bool				CheckFrame( int idx );

private:
	ITexture			**m_frame;
	int					m_frameCount;

	nsTexFrames();
};

#endif //_TexFrames_H_