// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file FileReader.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_FileReader_H_
#define	_FileReader_H_

#include "DataWriter.h"
#include "headers.h"

class nsFileReader : public IDataReader
{
public:
	explicit nsFileReader( const char *fileName, const char *mode = "rb" );
	~nsFileReader() override;

	bool	Read( void *data, uint size ) override;
	bool	Seek( long offset, int origin ) override;
	long	Tell() override;
	bool	IsValid() override;

private:
	FILE			*m_fp;
};

#endif //_FileReader_H_