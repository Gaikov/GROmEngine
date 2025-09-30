#include "Config.h"
#include "Package.h"
#include "AppInfo.h"
#include "nsLib/log.h"
#include "Args.h"
#include "sys.h"
#include "Engine/Platform.h"

nsConfig	*g_cfg = nullptr;

ICfgWriter	*nsConfig::m_writers[CFG_MAX_WRITERS];
int			nsConfig::m_writersCount = 0;

nsVar		*gc_savevars = nullptr;
nsVar		*developer = nullptr;

//---------------------------------------------------------
// writeconfig_f: 
//---------------------------------------------------------
static void writeconfig_f( int argc, const char *argv[] )
{
	if ( argc < 2 )
	{
		LogPrintf( PRN_ALL, "usage: writeconfig [filename]\n" );
		return;
	}

	g_cfg->SaveConfig( argv[1], true );
}

//---------------------------------------------------------
// quit_f:
//---------------------------------------------------------
static void quit_f( int argc, const char *argv[] )
{
	Sys_Exit();
}

//---------------------------------------------------------
// testcmd_f:
//---------------------------------------------------------
static void testcmd_f( int argc, const char *argv[] )
{
	LogPrintf( PRN_ALL, "===== test command =====\n" );
	for ( int i = 0; i < argc; i++ )
		LogPrintf( PRN_ALL, "arg: %s\n", argv[i] );
}

//---------------------------------------------------------
// fpu_state_f: 
//---------------------------------------------------------
static void fpu_state_f( int argc, const char *argv[] )
{
	Sys_FPUState();
}

//-----------------------------------------------------
//  nsConfig::nsConfig:  
//-----------------------------------------------------
nsConfig::nsConfig() :
        m_cfgFile(nullptr),
        m_defFile(nullptr),
        m_varList(nullptr) {

    auto app = App_GetInfo();
    auto platform = App_GetPlatform();

    IDataReader::sp_t in = app->GetCfgReader();
    if (!in || !in->IsValid()) {
        auto fileName = StrPrintf("%s.cfg", App_GetInfo()->GetAppName());
        //try to load from internal app data storage
        in = platform->InternalRead(fileName);
        if (!in || !in->IsValid()) {
            //try to load from app assets
            in = platform->FileRead(fileName);
        }
    }

    if (in && in->IsValid()) {
        m_cfgFile = IDataReader::ReadBlob(in);
    }

    m_defFile = g_pack.LoadFile(app->GetDefCfgPath());

    RegCmd("testcmd", testcmd_f);
    RegCmd("quit", quit_f);
    RegCmd("writeconfig", writeconfig_f);
    RegCmd("fpu_state", fpu_state_f);

    developer = RegVar("developer", "0", GVF_SAVABLE);
    gc_savevars = RegVar("gc_savevars", "0", GVF_SAVABLE);
}

//---------------------------------------------------------
// nsConfig::~nsConfig: 
//---------------------------------------------------------
nsConfig::~nsConfig()
{
	while ( m_varList )
	{
		nsVar *delVar = m_varList;
		m_varList = m_varList->m_next;
		delete delVar;
	}

	cmdDesc_t	*cmd;
	m_cmdHash.FetchBegin();
	while ( (cmd = m_cmdHash.FetchNext()) )
	{
		my_free( cmd->name );
		delete cmd;
	}
	m_cmdHash.FetchEnd();
	m_cmdHash.DeleteAllKeys();

	if ( m_cfgFile )
		g_pack.ReleaseFile( m_cfgFile );

	g_pack.ReleaseFile( m_defFile );
}

//-----------------------------------------------------
//  nsConfig::Init:  
//-----------------------------------------------------
bool nsConfig::Init()
{
	if ( !g_cfg )
		g_cfg = new nsConfig;

	return true;
}

//-----------------------------------------------------
//  nsConfig::Release:  
//-----------------------------------------------------
void nsConfig::Release()
{
	if ( g_cfg )
	{
		delete g_cfg;
		g_cfg = nullptr;
	}
}

//---------------------------------------------------------
// nsConfig::AddWriter: 
//---------------------------------------------------------
void nsConfig::AddWriter( ICfgWriter *writer )
{
	if ( !writer 
		|| m_writersCount >= CFG_MAX_WRITERS ) return;

	m_writers[ m_writersCount ] = writer;
	m_writersCount ++;
}

//-----------------------------------------------------
//  nsConfig::SaveConfig:  
//-----------------------------------------------------
void nsConfig::SaveConfig(const char *fileName, bool forceFile) {
    IDataWriter::sp_t out = App_GetInfo()->GetCfgWriter();
    if (forceFile || !out || !out->IsValid()) {
        if (!fileName)
            fileName = StrPrintf("%s.cfg", App_GetInfo()->GetAppName());

        Log::Info("...writing config '%s'", fileName);
        out = App_GetPlatform()->InternalWrite(fileName);
        if (!out->IsValid()) {
            Log::Warning("Failed to write config!");
            return;
        }
    }

    for (int i = 0; i < m_writersCount; i++)
        m_writers[i]->OnSaveConfig(out);

    for (nsVar *var = m_varList; var; var = var->m_next) {
        if ((var->m_flags & GVF_SAVABLE || gc_savevars->Bool())
            && !(var->m_flags & GVF_INTERNAL))
            out->Printf("%s \"%s\"\n", var->GetName(), var->String());
    }
}

//---------------------------------------------------------
// nsConfig::CompleteLine:
//---------------------------------------------------------
const char* nsConfig::CompleteLine( const char* line )
{
	Log::Info("Searching var/command: '%s'...", line);
	const char *outLine = nullptr;
	const int len = strlen(line);
	assert(len < nsString::MAX_SIZE);

	const auto *var = FindVar(line);
	if (var) return var->GetName();

	cmdDesc_t *cmd = m_cmdHash.GetData(line);
	if (cmd) return cmd->name;

	int refCount = 0;
	for (var = m_varList; var; var = var->m_next) {
		if (strstr(var->GetName(), line)) {
			refCount++;
			outLine = var->GetName();
			LogPrintf(PRN_ALL, " %s\n", outLine);
		}
	}

	//������� ������� �� ������� � �������
	m_cmdHash.FetchBegin();
	while ((cmd = m_cmdHash.FetchNext())) {
		if (strstr(cmd->name, line)) {
			refCount++;
			outLine = cmd->name;
			LogPrintf(PRN_ALL, " %s\n", outLine);
		}
	}
	m_cmdHash.FetchEnd();

	return refCount == 1 ? outLine : nullptr;
}

//---------------------------------------------------------
// nsConfig::GetDefaultConfig:
//---------------------------------------------------------
const char* nsConfig::GetDefaultConfig()
{
	if ( m_defFile )
		return (char*)m_defFile->GetData();
	return nullptr;
}

//---------------------------------------------------------
// nsConfig::GetCurrentConfig:
//---------------------------------------------------------
const char*	nsConfig::GetCurrentConfig()
{
	if ( m_cfgFile )
		return (char*)m_cfgFile->GetData();
	return nullptr;
}

//-----------------------------------------------------
// nsConfig::GetConfigValue:
//-----------------------------------------------------
const char* nsConfig::GetConfigValue( const char* varName, const char* cfgFileText )
{
	if ( !cfgFileText || !varName || !strlen( varName ) ) return nullptr;

	static nsString	tmpLine;
	const char			*line;

	const char	*begin = strstr( cfgFileText, varName );
	if ( !begin ) return nullptr;

	const char	*end = strchr( begin, '\n' );
	if ( !end )
		line = begin;
	else
	{
		int len = end - begin;
		tmpLine.CopyFrom( begin, len );
		line = tmpLine.AsChar();
	}

	nsArgs	args( line );
	if ( args.ArgCount() < 2
		|| !StrEqual( args.Arg( 0 ), varName ) ) return nullptr;

	tmpLine = args.Arg( 1 );
	return tmpLine;
}

//-----------------------------------------------------
//  nsConfig::RegisterVariable:  
//-----------------------------------------------------
nsVar* nsConfig::RegVar( const char* varName, const char* defValue, uint flags )
{
	if ( !varName || !defValue || !strlen( varName ) ) return nullptr;
	auto	*var = (nsVar*)FindVar( varName );
	if ( var ) return var;

	var = new nsVar( varName, defValue, flags );
	var->m_next = m_varList;
	m_varList = var;
	
	if ( (flags & GVF_INTERNAL) ) return var;

	const char *str = GetConfigValue(varName, GetDefaultConfig());
	if ( str )
	{
		var->m_defValue = str;
		var->SetDefault();
	}
	
	str = GetConfigValue( varName, GetCurrentConfig() );
	if ( str ) var->SetString( str );

	return var;
}

//-----------------------------------------------------
//  nsConfig::FindVariable:  
//-----------------------------------------------------
nsVar* nsConfig::FindVar( const char* varName )
{
	for ( nsVar *var = m_varList; var; var = var->m_next )
	{
		if ( StrEqual( var->GetName(), varName ) )
			return var;
	}
	return nullptr;
}

//-----------------------------------------------------
//  nsConfig::RegisterCommand:  
//-----------------------------------------------------
void nsConfig::RegCmd( const char* name, const func_t& cmd )
{
	if ( !name || !cmd || !strlen( name ) ) return;

	cmdDesc_t	*desc = m_cmdHash.GetData( name );
	if ( !desc )
	{
		desc = new cmdDesc_t;
		desc->name = my_strdup( name );
		desc->func = cmd;
		m_cmdHash.SetData( name, desc );
	}
}

//-----------------------------------------------------
//  nsConfig::ExecLine:  
//-----------------------------------------------------
bool nsConfig::ExecLine( const char* cmdLine )
{
	nsArgs		args( cmdLine );
	if ( !args.ArgCount() ) return false;
	
	nsVar*	var;
	cmdDesc_t*	cmd;

	if ( (var = FindVar( args.Arg( 0 ) )) )
	{
		if ( args.ArgCount() < 2 )
		{
			LogPrintf( PRN_ALL, "%s = %s (default: %s)\n", var->GetName(), var->String(), var->GetDefaultString() );
		}
		else if ( (var->m_flags & GVF_INTERNAL) || (var->m_flags & GVF_READONLY) )
			LogPrintf( PRN_ALL, "WARNING: read only variable!\n" );
		else
			var->SetString( args.Arg( 1 ) );
		return true;
	}
	else if ( (cmd = m_cmdHash.GetData( args.Arg( 0 ))) )
	{
		cmd->func( args.ArgCount(), args.GetArgs() );
		return true;
	}

	LogPrintf( PRN_ALL, "WARNING: '%s' - invalid variable or command name!\n", cmdLine );
	return false;
}

//---------------------------------------------------------
// nsConfig::ExecFile:
//---------------------------------------------------------
bool nsConfig::ExecFile( const char* fileName )
{
	nsFile	*file = g_pack.LoadFile( fileName );
	if ( !file ) return false;

	int		len;
	char	*line = StrToken( (char*)file->GetData(), "\r\n", len );
	if ( !line ) return false;

	nsString	*lines = nullptr;
	int			lineCount = 0;

	do 
	{
		char	prevCh = line[len];
		line[len] = 0;
		AddToArray( &lines, lineCount, nsString( line ) );
		line[len] = prevCh;
	}
	while ( (line = StrToken( nullptr, "\r\n", len )) );

	for ( int i = 0; i < lineCount; i++ )
		ExecLine( lines[i] );

	my_free( lines );
	return true;
}

bool nsConfig::SetString(const char *name, const char *value)
{
	auto var = FindVar(name);
	if (var)
	{
		var->SetString(value);
		return true;
	}
	return false;
}

const char *nsConfig::GetString(const char *name)
{
	auto var = FindVar(name);
	if (var)
	{
		LogPrintf( PRN_ALL, "WARNING: variable '%s' not found\n", name );
		return var->String();
	}
	return nullptr;
}

bool nsConfig::SetValue(const char *name, float value)
{
	auto var = FindVar(name);
	if (var)
	{
		var->SetValue(value);
		return true;
	}
	return false;
}

float nsConfig::GetValue(const char *name)
{
	auto var = FindVar(name);
	if (var)
	{
		LogPrintf( PRN_ALL, "WARNING: variable '%s' not found\n", name );
		return var->Value();
	}
	return 0;
}

