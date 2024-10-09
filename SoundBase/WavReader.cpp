// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file WavReader.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "WavReader.h"
#include "nsLib/log.h"

//---------------------------------------------------------
// nsWavReader::nsWavReader: 
//---------------------------------------------------------
nsWavReader::nsWavReader() :
	m_file(nullptr)
{

}

//---------------------------------------------------------
// nsWavReader::~nsWavReader: 
//---------------------------------------------------------
nsWavReader::~nsWavReader()
{
	Free();
}

//---------------------------------------------------------
// nsWavReader::Read: 
//---------------------------------------------------------
bool nsWavReader::Read( const char *fileName )
{
	Free();

	m_file = g_pack.LoadFile( fileName );
	if ( !m_file ) return false;

	if ( !Check( (char*)m_file->GetData() ) )
	{
		Free();
		return false;
	}

	auto fmt = GetFormat();

	uint size = *(uint*)(m_file->GetData() + 40);
	
	if ( size > m_file->GetSize() - 44 || size < 32 )
		size = ((m_file->GetSize() - 44) / fmt->nSamplesPerSec) * fmt->nSamplesPerSec;

	if ( size > m_file->GetSize() - 44 || size < 32 )
	{
		Free();
		return false;
	}

	m_samplesSize = size;

	return true;
}

//---------------------------------------------------------
// nsWavReader::Free: 
//---------------------------------------------------------
void nsWavReader::Free()
{
	if ( m_file )
		g_pack.ReleaseFile( m_file );
	m_file = nullptr;
	m_samplesSize = 0;
}

//---------------------------------------------------------
// nsWavReader::GetFormat: 
//---------------------------------------------------------
const nsWavReader::WaveFormat_t*	nsWavReader::GetFormat()
{
	if ( !m_file ) return nullptr;

	return (WaveFormat_t*)(m_file->GetData() + 20);
}

//---------------------------------------------------------
// nsWavReader::GetSamples: 
//---------------------------------------------------------
const void*	nsWavReader::GetSamples()
{
	if ( !m_file ) return nullptr;

	return m_file->GetData() + 44;
}

//---------------------------------------------------------
// nsWavReader::GetSamplesSize: 
//---------------------------------------------------------
uint nsWavReader::GetSamplesSize()
{
	if ( !m_file ) return 0;

	return m_samplesSize;
}

//---------------------------------------------------------
// nsWavReader::Check: 
//---------------------------------------------------------
bool nsWavReader::Check( const char *data )
{
	if ( strncmp( data, "RIFF", 4 ) == 0 )
		if ( strncmp( data + 12, "fmt", 3 ) == 0 )
			//if ( strncmp( data + 36, "data", 4 ) == 0 )
				return true;
	return false;
}
