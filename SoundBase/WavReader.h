// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file WavReader.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_WavReader_H_
#define	_WavReader_H_

#include "nsLib/comm_types.h"
#include "Core/Package.h"
#include "Core/headers.h"
#include <stdint.h>

class nsWavReader
{
public:
	typedef struct {
		uint16_t wFormatTag;
		uint16_t nChannels;
		uint32_t nSamplesPerSec;
		uint32_t nAvgBytesPerSec;
		uint16_t nBlockAlign;
		uint16_t wBitsPerSample;
		uint16_t cbSize;
	} WaveFormat_t;
public:
	nsWavReader();
	virtual ~nsWavReader();
	
	bool				Read( const char *fileName );
	void				Free();

	const WaveFormat_t*	GetFormat();
	const void*			GetSamples();
	uint				GetSamplesSize();

private:
	nsFile				*m_file;
	uint				m_samplesSize;

private:
	bool				Check( const char *data );

};

#endif //_WavReader_H_