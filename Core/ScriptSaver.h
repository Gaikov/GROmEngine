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
	void			Print(const char *line) const;
	void			PrintVar(const char *name, const char *fmt, ...) const;

	void			VarBool(const char *name, bool value, bool defValue) const;
	void			VarFloat(const char *name, float value, float defValue) const;
	void			VarFloat2(const char *name, const float value[2], const float defValue[2]) const;
	void			VarFloat3(const char *name, const float value[3], const float defValue[3]) const;
	void			VarFloat4(const char *name, const float value[4], const float defValue[4]) const;

	bool			BlockBegin(const char *name);
	void			BlockEnd();

private:
	FILE			*m_file;
	int				m_tabCount;
};

#endif //_ScriptSaver_H_