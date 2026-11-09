#include "Config.h"
#include "Package.h"
#include "AppInfo.h"
#include "nsLib/log.h"
#include "Args.h"
#include "sys.h"
#include "Engine/Platform.h"

nsConfig	*g_cfg = nullptr;

ICfgWriter	*nsConfig::_writers[CFG_MAX_WRITERS];
int			nsConfig::_writersCount = 0;

nsVar		*gc_savevars = nullptr;
nsVar		*developer = nullptr;

//---------------------------------------------------------
// writeconfig_f: 
//---------------------------------------------------------
static void writeconfig_f( int argc, const char *argv[] )
{
	if ( argc < 2 )
	{
		Log::Info("usage: writeconfig [filename]" );
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
	Log::Info("===== test command =====" );
	for ( int i = 0; i < argc; i++ )
		Log::Info("arg: %s", argv[i] );
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
        _cfgFile(nullptr),
        _defFile(nullptr),
        _varList(nullptr) {

    auto app = App_GetInfo();
    auto platform = App_GetPlatform();

    IDataReader::sp_t in = app->GetCfgReader();
    if (!in || !in->IsValid()) {
        auto fileName = StrPrintf("%s.cfg", app->GetAppName());
        //try to load from internal app data storage
        in = platform->InternalRead(fileName);
        if (!in || !in->IsValid()) {
            // Fall back to app assets, including pack files.
            _cfgFile = g_pack.LoadFile(fileName);
        }
    }

    if (in && in->IsValid()) {
        _cfgFile = IDataReader::ReadBlob(in);
    }

    _defFile = g_pack.LoadFile(app->GetDefCfgPath());

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
	while ( _varList )
	{
		nsVar *delVar = _varList;
		_varList = _varList->m_next;
		delete delVar;
	}

	cmdDesc_t	*cmd;
	_cmdHash.FetchBegin();
	while ( (cmd = _cmdHash.FetchNext()) )
	{
		my_free( cmd->name );
		delete cmd;
	}
	_cmdHash.FetchEnd();
	_cmdHash.DeleteAllKeys();

	if ( _cfgFile )
		g_pack.ReleaseFile( _cfgFile );

	g_pack.ReleaseFile( _defFile );
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
		|| _writersCount >= CFG_MAX_WRITERS ) return;

	_writers[ _writersCount ] = writer;
	_writersCount ++;
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

    for (int i = 0; i < _writersCount; i++)
        _writers[i]->OnSaveConfig(out);

    for (nsVar *var = _varList; var; var = var->m_next) {
        if ((var->m_flags & GVF_SAVABLE || gc_savevars->Bool())
            && !(var->m_flags & (GVF_INTERNAL | GVF_SESSION)))
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

	cmdDesc_t *cmd = _cmdHash.GetData(line);
	if (cmd) return cmd->name;

	int refCount = 0;
	for (var = _varList; var; var = var->m_next) {
		if (strstr(var->GetName(), line)) {
			refCount++;
			outLine = var->GetName();
			Log::Info(" %s", outLine);
		}
	}

	//������� ������� �� ������� � �������
	_cmdHash.FetchBegin();
	while ((cmd = _cmdHash.FetchNext())) {
		if (strstr(cmd->name, line)) {
			refCount++;
			outLine = cmd->name;
			Log::Info(" %s", outLine);
		}
	}
	_cmdHash.FetchEnd();

	return refCount == 1 ? outLine : nullptr;
}

//---------------------------------------------------------
// nsConfig::GetDefaultConfig:
//---------------------------------------------------------
const char* nsConfig::GetDefaultConfig()
{
	if ( _defFile )
		return (char*)_defFile->GetData();
	return nullptr;
}

//---------------------------------------------------------
// nsConfig::GetCurrentConfig:
//---------------------------------------------------------
const char*	nsConfig::GetCurrentConfig()
{
	if ( _cfgFile )
		return (char*)_cfgFile->GetData();
	return nullptr;
}

//-----------------------------------------------------
// nsConfig::GetConfigValue:
//-----------------------------------------------------
const char* nsConfig::GetConfigValue( const char* varName, const char* cfgFileText )
{
	if ( !cfgFileText || !varName || !strlen( varName ) ) return nullptr;

	static nsString tmpValue;
	const char *begin = cfgFileText;
	while ( *begin )
	{
		const char *end = strchr( begin, '\n' );
		const int length = end ? static_cast<int>(end - begin) : static_cast<int>(strlen(begin));
		nsString line;
		line.CopyFrom( begin, length );

		nsArgs args( line.AsChar() );
		if ( args.ArgCount() >= 2 && StrEqual( args.Arg( 0 ), varName ) )
		{
			tmpValue = args.Arg( 1 );
			return tmpValue;
		}

		if ( !end ) break;
		begin = end + 1;
	}

	return nullptr;
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
	var->m_next = _varList;
	_varList = var;
	
	if ( flags & (GVF_INTERNAL | GVF_SESSION) ) return var;

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
	for ( nsVar *var = _varList; var; var = var->m_next )
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

	cmdDesc_t	*desc = _cmdHash.GetData( name );
	if ( !desc )
	{
		desc = new cmdDesc_t;
		desc->name = my_strdup( name );
		desc->func = cmd;
		_cmdHash.SetData( name, desc );
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
			Log::Info("%s = %s (default: %s)", var->GetName(), var->String(), var->GetDefaultString() );
		}
		else if ( (var->m_flags & GVF_INTERNAL) || (var->m_flags & GVF_READONLY) )
			Log::Warning("read only variable!" );
		else
			var->SetString( args.Arg( 1 ) );
		return true;
	}
	else if ( (cmd = _cmdHash.GetData( args.Arg( 0 ))) )
	{
		cmd->func( args.ArgCount(), args.GetArgs() );
		return true;
	}

	Log::Warning("'%s' - invalid variable or command name!", cmdLine );
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
		Log::Warning("variable '%s' not found", name );
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
	if (!var)
	{
		Log::Warning("variable '%s' not found", name);
		return 0;
	}
	return var->Value();
}
