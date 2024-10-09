// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file DataWriter.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_DataWriter_H_
#define	_DataWriter_H_

#include "nsLib/comm_types.h"
#include "SmartPtr.h"
#include "Blob.h"

/*=======================================================================*\
	��������� ������������� ��� ���������� �����������/���������� �����
	����, ��� ������� (�����) ������ ����������� ����� g_pack (nsPackage).
	� ��� ����� ������������� ��� ���������� �������� ���������������� ������
	(������, �������, �����, �������� � �.�.)
\*=======================================================================*/

//---------------------------------------------------------
// IDataWriter: 
//---------------------------------------------------------
struct IDataWriter
{
	typedef nsSmartPtr<IDataWriter>	sp_t;
	
	virtual ~IDataWriter() = default;
	virtual bool        	Printf( const char *fmt, ... ) = 0;
	virtual bool			Write( const void *data, uint size ) = 0;
	virtual bool			IsValid() = 0;
	
	// origin - ����������� ���������:
	// SEEK_CUR - Current position of file pointer
	// SEEK_END - End of file
	// SEEK_SET - Beginning of file
	virtual bool			Seek( long offset, int origin ) = 0;
	
	virtual long			Tell() = 0;
};

//---------------------------------------------------------
// IDataReader: 
//---------------------------------------------------------
struct IDataReader
{
	typedef nsSmartPtr<IDataReader>	sp_t;

	virtual ~IDataReader() = default;
	virtual bool			Read( void *data, uint size ) = 0;
	
	// origin - ����������� ���������:
	// SEEK_CUR - Current position of file pointer
	// SEEK_END - End of file
	// SEEK_SET - Beginning of file
	virtual bool			Seek( long offset, int origin ) = 0;

	virtual long			Tell() = 0;
	
	virtual bool			IsValid() = 0;

    static nsFile*          ReadBlob(IDataReader *reader);
};

#endif //_DataWriter_H_