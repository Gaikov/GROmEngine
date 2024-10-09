// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Var.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_Var_H_
#define	_Var_H_

#include "nsLib/comm_types.h"
#include "nsLib/events/EventDispatcher.h"

#define	GVF_SAVABLE		1u	//����������� � config.cfg, ���� cfg_savevars 1 - ����������� ��� ������ GVF_INTERNAL
#define GVF_READONLY	2u	//���������� �� ��������������� � �������
#define GVF_INTERNAL	4u	//���������� ��� ����������� ������������� (��� �������� � ��������, ��������...)


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
	virtual ~nsVar();
};

#endif //_Var_H_