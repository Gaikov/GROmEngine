// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Var.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/comm_types.h"
#include "nsLib/events/EventDispatcher.h"

#define	GVF_SAVABLE		1u	//gets saved into config always
#define GVF_READONLY	2u	//readonly and does not get saved into config except gc_savevars is set
#define GVF_INTERNAL	4u	//never gets saved into config file and readonly
//empty flags means that a var will not be saved except gc_savevars is set (debugging vars for example)

//-----------------------------------------------------
//  class nsVar:  
//-----------------------------------------------------
class nsVar : public nsEventDispatcher
{
friend class nsConfig;
public:
    enum { NSVAR_CHANGED = 0 };

public:
	inline const char*	String() { return m_currValue; }
	inline float		Value() { return m_value; }
	bool                Bool() { return m_value != 0.0f; }
	
	void				SetValue( float val );
	void				SetString( const char* str );

	float				GetDefaultValue();
	const char*			GetDefaultString();
	void				SetDefault();

	const char*			GetName() { return m_name; }

	void				Inc() { m_value += 1.0f; }
	void				Dec() { m_value -= 1.0f; }
    void                SetNumber( float v ) { m_value = v; }

private:
	char				*m_name;
	uint				m_flags;
	
	float				m_value;
	char				*m_defValue;
	char				*m_currValue;

	nsVar				*m_next;

private:
	nsVar( const char* name, const char* defValue, uint flags );
	~nsVar() override;
};