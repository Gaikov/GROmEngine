// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParseFile.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "ParseFile.h"
#include "nsLib/StrTools.h"
#include "StructUt.h"
#include "nsLib/log.h"

//---------------------------------------------------------
// nsParseFile::nsParseFile: 
//---------------------------------------------------------
nsParseFile::nsParseFile() :
	m_files( 0 ),
	m_fileCount( 0 )	
{
	
}

//---------------------------------------------------------
// nsParseFile::~nsParseFile: 
//---------------------------------------------------------
nsParseFile::~nsParseFile()
{
	if ( !m_files ) return;

	for ( int i = 0; i < m_fileCount; ++i )
	{
		if ( m_files[i].ss )
			ps_end( m_files[i].ss );

		if ( m_files[i].file )
			g_pack.ReleaseFile( m_files[i].file );
	}

	my_free( m_files );
}

//---------------------------------------------------------
// nsParseFile::BeginFile: 
//---------------------------------------------------------
script_state_t* nsParseFile::BeginFile( const char *fileName, char prefix )
{
	if ( !StrCheck( fileName ) ) return 0;

	scriptFile_t	script = {};
	MemZero( script );

	script.file = g_pack.LoadFile( fileName );
	if ( !script.file ) return nullptr;
	
	script.ss = ps_begin( (char*)script.file->GetData(), prefix );

	AddToArray( &m_files, m_fileCount, script );
	return script.ss;
}
