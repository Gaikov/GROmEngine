// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Name.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "Name.h"
#include "StructUt.h"
#include "nsLib/StrTools.h"

//---------------------------------------------------------
// nsName::nsName: 
//---------------------------------------------------------
nsName::nsName() :
	m_id( -1 )
{

}

//---------------------------------------------------------
// nsName::nsName: 
//---------------------------------------------------------
nsName::nsName( const char *name )
{
	m_id = nsNamePool::GetID( name );
}

//---------------------------------------------------------
// nsName::AsInt: 
//---------------------------------------------------------
int	nsName::AsInt() const
{
	return m_id;
}

//---------------------------------------------------------
// nsName::AsChar: 
//---------------------------------------------------------
const char*	nsName::AsChar() const
{
	return nsNamePool::GetNameByID( m_id );
}

//---------------------------------------------------------
// nsName::IsValid: 
//---------------------------------------------------------
bool nsName::IsValid() const
{
	return nsNamePool::IsValidID( m_id );
}

//---------------------------------------------------------
// nsName::operator ==: 
//---------------------------------------------------------
bool nsName::operator == ( const nsName &name ) const
{
	return m_id == name.m_id;
}

//---------------------------------------------------------
// nsName::operator !=: 
//---------------------------------------------------------
bool nsName::operator != ( const nsName &name ) const
{
	return m_id != name.m_id;
}

char**	nsNamePool::m_names = 0;
int		nsNamePool::m_count = 0;

//---------------------------------------------------------
// nsNamePool::GetID: 
//---------------------------------------------------------
int	nsNamePool::GetID( const char *name )
{
	if ( !name ) return -1;

	for ( int i = 0; i < m_count; ++i )
	{
		if ( StrEqual( m_names[i], name ) )
			return i;
	}

	AddToArray( &m_names, m_count, my_strdup( name ) );
	return m_count - 1;
}

//---------------------------------------------------------
// nsNamePool::GetNameByID: 
//---------------------------------------------------------
const char*	nsNamePool::GetNameByID( int id )
{
	if ( !IsValidID( id ) ) return "";
	return m_names[id];
}

//---------------------------------------------------------
// nsNamePool::IsValidID: 
//---------------------------------------------------------
bool nsNamePool::IsValidID( int id )
{
	return id >= 0 && id < m_count;
}

//---------------------------------------------------------
// nsNamePool::Free: 
//---------------------------------------------------------
void nsNamePool::Free()
{
	for ( int i = 0; i < m_count; ++i )
		my_free( m_names[i] );

	my_free( m_names );
}
