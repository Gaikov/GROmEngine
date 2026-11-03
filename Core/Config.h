// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Config.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "headers.h"
#include "Var.h"
#include "HashMap.h"
#include "DataWriter.h"
#include "nsLib/SubSystem.h"

class nsFile;

//---------------------------------------------------------
// cmdDesc_t: 
//---------------------------------------------------------
typedef std::function<void( int argc, const char *argv[] )> func_t;

struct cmdDesc_t
{
	char*	name;
	func_t	func;
};

//---------------------------------------------------------
// nsConfigWriter: 
//---------------------------------------------------------
#define CFG_MAX_WRITERS	16

struct ICfgWriter
{
public:
	virtual void OnSaveConfig( IDataWriter *out ) = 0;
};

//-----------------------------------------------------
//  class nsConfig:  
//-----------------------------------------------------
class nsConfig
{
public:
	static bool				Init();
	static void				Release();

	static void				AddWriter( ICfgWriter *writer );
	void					SaveConfig( const char* fileName = 0, bool forceFile = false );

	const char*				CompleteLine( const char* line );
	
	const char*				GetDefaultConfig();
	const char*				GetCurrentConfig();

	nsVar*					RegVar( const char* varName, const char* defValue, uint flags = 0 );
	nsVar*					FindVar( const char* varName );

	bool                    SetString(const char *name, const char *value);
	const char*             GetString(const char *name);

	bool                    SetValue(const char *name, float value);
	float                   GetValue(const char *name);

	void					RegCmd( const char* name, const func_t& cmd );
	const cmdDesc_t*		FindCmd( const char* cmdName ) { return _cmdHash.GetData( cmdName ); }
		
	bool					ExecLine( const char* cmdLine );
	bool					ExecFile( const char* fileName );

private:
    nsVar *_varList;
    nsHashMap<cmdDesc_t> _cmdHash;
    nsFile *_cfgFile;
    nsFile *_defFile;

    static ICfgWriter *_writers[CFG_MAX_WRITERS];
    static int _writersCount;

	

private:
	nsConfig();
	virtual ~nsConfig();

	static const char*				GetConfigValue( const char* varName, const char* cfgFileText );
};

extern nsConfig	*g_cfg;
