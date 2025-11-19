// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ScriptSaver.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_ScriptSaver_H_
#define	_ScriptSaver_H_

#include "headers.h"

class nsScriptSaver
{
public:
	enum { MAX_OUT_LEN = 1024 };

public:
	explicit nsScriptSaver( const char *fileName );
	virtual ~nsScriptSaver();

    bool            IsValid();

	void        	Printf( const char *fmt, ... );
	bool			BlockBegin(const char *name);
	void			BlockEnd();

private:
	FILE			*m_file;
	int				m_tabCount;
};

#endif //_ScriptSaver_H_