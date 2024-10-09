// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file FileWriter.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_FileWriter_H_
#define	_FileWriter_H_

#include "DataWriter.h"
#include "headers.h"

class nsFileWriter : public IDataWriter
{
public:
	explicit nsFileWriter( const char *fileName, const char *mode = "wb" );
    ~nsFileWriter() override;

	bool        	Printf( const char *fmt, ... ) override;
	bool			Write( const void *data, uint size ) override;
	bool			IsValid() override { return m_fp != nullptr; }
	bool			Seek( long offset, int origin ) override;
	long			Tell() override;

private:
	FILE					*m_fp;
};

#endif //_FileWriter_H_
