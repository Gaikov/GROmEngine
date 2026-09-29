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
nsArgs::nsArgs() = default;

//---------------------------------------------------------
// nsArgs::nsArgs:
//---------------------------------------------------------
nsArgs::nsArgs( const char *line ) {
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
	return _argCount;
}

//---------------------------------------------------------
// nsArgs::Arg:
//---------------------------------------------------------
const char *nsArgs::Arg( int idx ) const
{
	if ( idx < 0 || idx >= _argCount ) return nullptr;
	return _args[idx];
}

//---------------------------------------------------------
// nsArgs::GetArgs:
//---------------------------------------------------------
const char** nsArgs::GetArgs() const
{
	return const_cast<const char **>( _args );
}

//---------------------------------------------------------
// nsArgs::FromArgs:
//---------------------------------------------------------
void nsArgs::FromArgs( int argc, const char *argv[] )
{
	Free();
	for ( int i = 0; i < argc; i++ )
	{
		char	*arg = my_strdup( argv[i] );
		AddToArray( &_args, _argCount, arg );
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
			AddToArray( &_args, _argCount, params[i].str );
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
		AddToArray( &_args, _argCount, arg );

		token = StrToken( nullptr, " \t\n\r", len );
	}
}

//---------------------------------------------------------
// nsArgs::Free:
//---------------------------------------------------------
void nsArgs::Free()
{
	if ( _args )
	{
		for ( int i = 0; i < _argCount; i++ )
			my_free( _args[i] );
		my_free( _args );
		_args = nullptr;
		_argCount = 0;
	}	
}

bool nsArgs::HasArg( const char *value ) const
{
	for (int i = 0; i < _argCount; i++)
	{
		if (StrEqual(value, _args[i]))
		{
			return true;
		}
	}

	return false;
}

void nsArgs::Clear() {
	Free();
}

//---------------------------------------------------------
// nsArgs::GetValue: value following the first named arg.
//---------------------------------------------------------
const char *nsArgs::GetValue( const char *name ) const {
	if ( !name || !*name ) return nullptr;
	for ( auto i = 0; i + 1 < _argCount; ++i ) {
		if ( StrEqual( name, _args[i] ) ) return _args[i + 1];
	}
	return nullptr;
}


