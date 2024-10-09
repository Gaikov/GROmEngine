// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ResourceMap.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "ResourceMap.h"
#include "nsLib/log.h"
#include "Core/Memory.h"
#include "Core/StructUt.h"
#include "Core/Package.h"

nsResourceMap g_resMap;

//---------------------------------------------------------
// nsResourceMap::nsResourceMap: 
//---------------------------------------------------------
nsResourceMap::nsResourceMap() :
	m_texMap( 0 ),
	m_texCount( 0 ),
	m_sndMap( 0 ),
	m_sndCount( 0 )
{

}

//---------------------------------------------------------
// nsResourceMap::Init: 
//---------------------------------------------------------
bool nsResourceMap::Init()
{
	LogPrintf( PRN_ALL, "...init resmap\n" );
	nsFile	*file = g_pack.LoadFile( "scripts/resmap.txt" );
	if ( !file ) return true;

	script_state_t *ss = ps_begin( (char*)file->GetData() );

	ParseRes( &m_texMap, m_texCount, ss, "texture" );
	LogPrintf( PRN_ALL, " %i textures id parsed\n", m_texCount );

	ParseRes( &m_sndMap, m_sndCount, ss, "sound" );
	LogPrintf( PRN_ALL, " %i sounds id parsed\n", m_sndCount );

	ps_end( ss );
	g_pack.ReleaseFile( file );
	return true;
}

//---------------------------------------------------------
// nsResourceMap::Release: 
//---------------------------------------------------------
void nsResourceMap::Release()
{
	if ( m_texMap )
		my_free( m_texMap );
	m_texMap = 0;
	m_texCount = 0;

	if ( m_sndMap )
		my_free( m_sndMap );
	m_sndMap = 0;
	m_sndCount = 0;
}

//---------------------------------------------------------
// nsResourceMap::TextureFile: 
//---------------------------------------------------------
const char*	nsResourceMap::TextureFile( const char *fileNameOrID )
{
	if ( !StrCheck( fileNameOrID ) ) return 0;

	texMap_t	*res = FindRes( m_texMap, m_texCount, fileNameOrID );
	if ( res )
		return res->fileName;

	return fileNameOrID;
}

//---------------------------------------------------------
// nsResourceMap::ParseRes: 
//---------------------------------------------------------
template <class T>
bool nsResourceMap::ParseRes( T **map, int &count, script_state_t *ss, const char *blockName )
{
	T	*resMap = 0;
	count = 0;

	if ( ps_block_begin( ss, blockName ) )
	{
		do
		{
			T	res;
			res.id = ps_get_str( ss, "id" );
			res.fileName = ps_get_str( ss, "file" );

			AddToArray( &resMap, count, res );
		}
		while( ps_block_next( ss ) );
		ps_block_end( ss );
	}

	*map = resMap;
	return count != 0;
}

//---------------------------------------------------------
// nsResourceMap::FindRes: 
//---------------------------------------------------------
template <class T>
T* nsResourceMap::FindRes( T *map, int count, const char *id )
{
	for ( int i = 0; i < count; ++i )
		if ( map[i].id == id )
			return &map[i];
	return 0;	
}

