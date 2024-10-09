// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Args.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "Args.h"
#include "Memory.h"
#include "StructUt.h"
#include "nsLib/StrTools.h"

//---------------------------------------------------------
// nsArgs::nsArgs:
//---------------------------------------------------------
nsArgs::nsArgs() :
	m_argCount( 0 ),
	m_args( nullptr )
{

}

//---------------------------------------------------------
// nsArgs::nsArgs:
//---------------------------------------------------------
nsArgs::nsArgs( const char* line ) :
	m_argCount( 0 ),
	m_args( nullptr )
{
	FromLine( line );
}

//---------------------------------------------------------
// nsArgs::~nsArgs:
//---------------------------------------------------------
nsArgs::~nsArgs()
{
	Free();
}

//---------------------------------------------------------
// nsArgs::ArgCount:
//---------------------------------------------------------
int nsArgs::ArgCount() const
{
	return m_argCount;
}

//---------------------------------------------------------
// nsArgs::Arg:
//---------------------------------------------------------
const char*	nsArgs::Arg( int idx ) const
{
	if ( idx < 0 || idx >= m_argCount ) return nullptr;
	return m_args[idx];
}

//---------------------------------------------------------
// nsArgs::GetArgs:
//---------------------------------------------------------
const char** nsArgs::GetArgs() const
{
	return (const char**)m_args;
}

//---------------------------------------------------------
// nsArgs::FromArgs:
//---------------------------------------------------------
void nsArgs::FromArgs( int argc, const char* argv[] )
{
	for ( int i = 0; i < argc; i++ )
	{
		char	*arg = my_strdup( argv[i] );
		AddToArray( &m_args, m_argCount, arg );
	}
}

//---------------------------------------------------------
// nsArgs::FromLine:
//---------------------------------------------------------
struct param_t
{
	char	*str;
	bool	whole;	//true - �� ������ �� �����
};

void nsArgs::FromLine( const char *line )
{
	Free();
	if ( !line || !strlen( line ) ) return;

	param_t	*params = nullptr;
	int		count = 0;
	bool	second = false;
	param_t	p{};

	const char	*from = line;
	while ( from )
	{
		bool	add = false;
		const char	*found = strchr( from, '"' );
		if ( !found )
		{
			if ( strlen( from ) )
			{
				p.str = my_strdup( from );
				p.whole = second;
				add = true;
			}
			from = nullptr;
		}
		else
		{
			int	len = found - from;
			if ( len )
			{
				p.str = (char*)my_malloc( len + 1 );
				strncpy( p.str, from, len );
				p.whole = second;
				add = true;
			}

			second = !second;
			from = found + 1;
		}

		if ( add ) AddToArray( &params, count, p );
	}

	for ( int i = 0; i < count; i++ )
	{
		if ( params[i].whole )
			AddToArray( &m_args, m_argCount, params[i].str );
		else
		{
			AddFromLine( params[i].str );
			my_free( params[i].str );
		}
	}

	if ( params ) my_free( params );
}

//---------------------------------------------------------
// nsArgs::AddFromLine: 
// ��������� ��������� � ������ ����������
// ������ ����� �� ������
//---------------------------------------------------------
void nsArgs::AddFromLine( const char* line )
{
	if ( !line || !strlen( line ) ) return;

	int		len;
	char*	token = StrToken( line, " \t\n\r", len );
	while ( token )
	{
		char	*arg = (char*)my_malloc( len + 1 );
		strncpy( arg, token, len );
		arg[len] = 0;
		AddToArray( &m_args, m_argCount, arg );

		token = StrToken( nullptr, " \t\n\r", len );
	}
}

//---------------------------------------------------------
// nsArgs::Free:
//---------------------------------------------------------
void nsArgs::Free()
{
	if ( m_args )
	{
		for ( int i = 0; i < m_argCount; i++ )
			my_free( m_args[i] );
		my_free( m_args );
		m_args = nullptr;
		m_argCount = 0;
	}	
}

bool nsArgs::HasArg(const char *value) const
{
	for (int i = 0; i < m_argCount; i++)
	{
		if (StrEqual(value, m_args[i]))
		{
			return true;
		}
	}

	return false;
}



