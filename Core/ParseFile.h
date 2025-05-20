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

class nsParseBlock {
public:
	explicit nsParseBlock(script_state_t *ss);
	~nsParseBlock();
	bool Begin(const char *blockName);
	void End();

private:
	script_state_t	*_ss;
	int _openedBlocks = 0;
};

#endif //_ParseFile_H_