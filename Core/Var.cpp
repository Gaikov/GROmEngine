// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Var.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "Var.h"
#include "Memory.h"
#include "nsLib/StrTools.h"

//-----------------------------------------------------
//  nsVar::nsVar:  
//-----------------------------------------------------
nsVar::nsVar( const char* name, const char* defValue, uint flags ) :
	m_next( nullptr )
{
	assert( name != nullptr && defValue != nullptr );
	m_name = name;
	m_defValue = defValue;
	m_flags = flags;
	SetDefault();
}

//-----------------------------------------------------
//  nsVar::SetValue:  
//-----------------------------------------------------
void nsVar::SetValue( float val )
{
	if ( val == (float)(int)val )
		SetString( StrPrintf( "%i", (int)val) );
	else
		SetString( StrPrintf( "%f", val ) );
}

//-----------------------------------------------------
//  nsVar::SetString:  
//-----------------------------------------------------
void nsVar::SetString( const char* str )
{
    if (str == m_currValue) {
        return;
    }

	if ( !str ) str = "";

	m_currValue = str;
	m_value = (float)atof( String() );
    Emmit(nsBaseEvent(NSVAR_CHANGED));
}

//-----------------------------------------------------
//  nsVar::GetDefaultValue:  
//-----------------------------------------------------
float nsVar::GetDefaultValue()
{
	return (float)atof( m_defValue.c_str() );
}

//-----------------------------------------------------
//  nsVar::GetDefaultString:  
//-----------------------------------------------------
const char* nsVar::GetDefaultString()
{
	return m_defValue.c_str();
}

//---------------------------------------------------------
// nsVar::SetDefault: 
//---------------------------------------------------------
void nsVar::SetDefault()
{
	SetString( m_defValue.c_str() );
}