// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file FileWriter.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "FileWriter.h"
#include "nsLib/StrTools.h"
#include "nsLib/log.h"

//---------------------------------------------------------
// nsFileWriter::nsFileWriter: 
//---------------------------------------------------------
nsFileWriter::nsFileWriter(const char *fileName, const char *mode) :
        m_fp(nullptr) {
    if (StrCheck(fileName)) {
        m_fp = fopen(fileName, mode);
        if (!m_fp)
            LogPrintf(PRN_ALL, "WARNING: can't create file '%s'\n", fileName);
    }
}
	
//---------------------------------------------------------
// nsFileWriter::~nsFileWriter: 
//---------------------------------------------------------
nsFileWriter::~nsFileWriter()
{
	if ( m_fp )
		fclose( m_fp );
}

//---------------------------------------------------------
// nsFileWriter::Printf: 
//---------------------------------------------------------
bool nsFileWriter::Printf( const char *fmt, ... )
{
	if ( !m_fp || !StrCheck( fmt ) ) return false;
	va_list	list;
	va_start( list, fmt );
	vfprintf( m_fp, fmt, list );
	va_end( list );
	return true;
}

//---------------------------------------------------------
// nsFileWriter::Write: 
//---------------------------------------------------------
bool nsFileWriter::Write( const void *data, uint size )
{
	if ( !m_fp || !data || !size ) return false;
	return fwrite( data, size, 1, m_fp ) == 1;
}

//---------------------------------------------------------
// nsFileWriter::Seek: 
//---------------------------------------------------------
bool nsFileWriter::Seek( long offset, int origin )
{
	if ( !m_fp ) return false;
	return fseek( m_fp, offset, origin ) == 0;
}

//---------------------------------------------------------
// nsFileWriter::GetCurrSize: 
//---------------------------------------------------------
long nsFileWriter::Tell()
{
	if ( !m_fp ) return 0;
	return ftell( m_fp );
}

