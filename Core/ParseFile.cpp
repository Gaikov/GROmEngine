// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParseFile.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "ParseFile.h"
#include "nsLib/StrTools.h"
#include "StructUt.h"

//---------------------------------------------------------
// nsParseFile::~nsParseFile: 
//---------------------------------------------------------
nsParseFile::~nsParseFile()
{
	for (auto &file : m_files)
	{
		if ( file.ss )
            ps_end(file.ss);

		if ( file.file )
			g_pack.ReleaseFile( file.file );
	}
}

//---------------------------------------------------------
// nsParseFile::BeginFile: 
//---------------------------------------------------------
script_state_t* nsParseFile::BeginFile( const char *fileName, char prefix )
{
	if ( !StrCheck( fileName ) ) return nullptr;

	scriptFile_t	script = {};
	MemZero( script );

	script.file = g_pack.LoadFile( fileName );
	if ( !script.file ) return nullptr;
	
	script.ss = ps_begin( (char*)script.file->GetData(), prefix );

	m_files.push_back(script);
	return script.ss;
}

nsParseBlock::nsParseBlock(script_state_t *ss) : _ss(ss) {
}

nsParseBlock::~nsParseBlock() {
	while (_openedBlocks > 0) {
		ps_block_end(_ss);
		--_openedBlocks;
	}
}

bool nsParseBlock::Begin(const char *blockName) {
	if (ps_block_begin(_ss, blockName)) {
		_openedBlocks ++;
		return true;
	}
	return false;
}

void nsParseBlock::End() {
	if (_openedBlocks > 0) {
		_openedBlocks --;
		ps_end(_ss);
	}
}
