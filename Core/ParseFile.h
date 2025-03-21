// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParseFile.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_ParseFile_H_
#define	_ParseFile_H_

#include "Parser.h"
#include "Package.h"

class nsParseFile
{
public:
    nsParseFile() = default;
	virtual			~nsParseFile();
	script_state_t* BeginFile( const char *fileName, char prefix = '$' );

private:
	struct scriptFile_t
	{
		nsFile			*file;
		script_state_t	*ss;
	};

private:
	std::vector<scriptFile_t>   m_files;

private:
	//����� ����������
	nsParseFile( const nsParseFile& );
	const nsParseFile& operator = ( const nsParseFile& );
};

#endif //_ParseFile_H_