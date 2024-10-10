#include "sys.h"
#include "nsLib/log.h"
#include "nsLib/StrTools.h"
#include <sys/stat.h>
#include "AppInfo.h"
#include "nsLib/FilePath.h"
#include "Engine/Platform.h"

//---------------------------------------------------------
// Sys_FatalError: 
//---------------------------------------------------------
void  Sys_FatalError( const char *fmt, ... )
{
	va_list	list;
	char	msg[MAX_MSG];
	
	va_start( list, fmt );
	vsprintf( msg, fmt, list );
	va_end( list );

	nsString	cap;
	cap.Format( "%s: FatalError", App_GetInfo()->GetAppName() );
	
	LogPrintf( PRN_ALL, "FATAL ERROR: %s\n", msg );
    App_GetPlatform()->MessagePopup(cap, msg);
	Sys_Exit();
}

//-----------------------------------------------------
// Sys_Message:
//-----------------------------------------------------
void  Sys_Message( const char *fmt, ... )
{
	va_list	list;
	char	msg[MAX_MSG];
	
	va_start( list, fmt );
	vsprintf( msg, fmt, list );
	va_end( list );

	nsString	cap;
	cap.Format( "%s: Message", App_GetInfo()->GetAppName() );
	
	LogPrintf( PRN_ALL, "MESSAGE: %s\n", msg );
    App_GetPlatform()->MessagePopup(cap, msg);
}

//---------------------------------------------------------
// Sys_CPUSpeed: 
//---------------------------------------------------------
double Sys_CPUSpeed() 
{ 
	//DWORD dwTimerHi = 0, dwTimerLo = 0;
	double dRes = 0;

	/*__asm
	{ 
		rdtsc
		mov dwTimerLo, EAX 
		mov dwTimerHi, EDX 
	} 

    Sleep (500); 

    __asm
	{ 
		rdtsc
		sub EAX, dwTimerLo 
		sub EAX, dwTimerHi 
		mov dwTimerLo, EAX 
		mov dwTimerHi, EDX 
	}//*/

	//dRes = dwTimerLo / ( 1000.0 * 500 );
	return dRes; 
}

//---------------------------------------------------------
// Sys_OSName: 
//---------------------------------------------------------
std::string Sys_OSName()
{
#ifdef _WINDOWS
	std::string osName;
	std::string	name;
	char	ver[256];
	memset( ver, 0, sizeof(ver) );

	OSVERSIONINFO	oi;

	memset( &oi, 0, sizeof(OSVERSIONINFO) );
	oi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if ( !GetVersionEx( &oi ) ) return 0;

	switch ( oi.dwPlatformId )
	{
	case VER_PLATFORM_WIN32s:
		name = "Windows 3.1";
		break;
	case VER_PLATFORM_WIN32_WINDOWS:
		name = oi.dwMinorVersion ? "Windows 98" : "Windows 95";
		break;
	case VER_PLATFORM_WIN32_NT:
		name = "Windows NT";
		sprintf( ver, " ver %i.%i", oi.dwMajorVersion, oi.dwMinorVersion );
		break;
	default:
		name = "Unknown";
		break;
	}

	name += ver;
	return name;
#endif
	return "Linux";
}

//---------------------------------------------------------
// Sys_SetMaxPriority: 
//---------------------------------------------------------
void Sys_SetMaxPriority()
{
	LogPrintf(PRN_ALL, "Sys_SetMaxPriority - not implemented\n");
}

//---------------------------------------------------------
// Sys_SetNormalPriority: 
//---------------------------------------------------------
void Sys_SetNormalPriority()
{
	LogPrintf(PRN_ALL, "Sys_SetNormalPriority - not implemented\n");
}

//-----------------------------------------------------
//  Sys_MakePath:  
//	������� ���� ����������, ��������� ���:
//	dir1/dir2/dir3
//-----------------------------------------------------
bool Sys_MakePath( const char* dirPath )
{
	if ( !dirPath || !strlen( dirPath ) ) return false;

	char	str[nsString::MAX_SIZE];
	strncpy( str, dirPath, nsString::MAX_SIZE - 1 );

	char	*slash = str;
	do
	{
		slash = strchr( slash, '/' );
		if ( slash ) *slash = 0;
		if ( !nsFilePath::MakeFolder(str) )
		{
			LogPrintf( PRN_ALL, "WARNING: can't create directory '%s'\n", str );
			return false;
		}
		
		if ( slash )
		{
			*slash = '/';
			slash ++;
		}
	}
	while ( slash );

	return true;
}

//-----------------------------------------------------
//  SysExit:  
//-----------------------------------------------------
static bool g_isExit = false;

void Sys_Exit()
{
	//PostQuitMessage( 0 );
	//assert(!"not implemented");
	g_isExit = true;
}

//---------------------------------------------------------
// Sys_IsExit:
//---------------------------------------------------------
bool Sys_IsExit()
{
	return g_isExit;
}

//---------------------------------------------------------
// Sys_FPUState: 
//---------------------------------------------------------

typedef struct
{
	const char	*group;
	uint		mask;
	const char	*name;
	uint		value;
}
FPU_Flag_t;

static FPU_Flag_t	flags[] = 
{
	/*{ "Denormal control", _MCW_DN, "_DN_SAVE", _DN_SAVE },
	{ 0, _MCW_DN, "_DN_FLUSH", _DN_FLUSH },

	{ "Interrupt exception", _MCW_EM, "_EM_INVALID", _EM_INVALID },
	{ 0, _MCW_EM, "_EM_DENORMAL", _EM_DENORMAL },
	{ 0, _MCW_EM, "_EM_ZERODIVIDE", _EM_ZERODIVIDE },
	{ 0, _MCW_EM, "_EM_OVERFLOW", _EM_OVERFLOW },
	{ 0, _MCW_EM, "_EM_UNDERFLOW", _EM_UNDERFLOW },
	{ 0, _MCW_EM, "_EM_INEXACT", _EM_INEXACT },
	
	{ "Infinity control", _MCW_IC, "_IC_AFFINE", _IC_AFFINE },
	{ 0, _MCW_IC, "_IC_PROJECTIVE", _IC_PROJECTIVE },

	{ "Rounding control", _MCW_RC, "_RC_CHOP", _RC_CHOP },
	{ 0, _MCW_RC, "_RC_UP", _RC_UP },
	{ 0, _MCW_RC, "_RC_DOWN", _RC_DOWN },
	{ 0, _MCW_RC, "_RC_NEAR", _RC_NEAR },
	
	{ "Precision control", _MCW_PC, "_PC_24", _PC_24 },
	{ 0, _MCW_PC, "_PC_53", _PC_53 },
	{ 0, _MCW_PC, "_PC_64", _PC_64 }*/
};

static int	flagsCount = sizeof(flags) / sizeof(FPU_Flag_t);

void Sys_FPUState()
{
	LogPrintf( PRN_ALL, "============= FPU flags ===============\n" );
	/*unsigned int state = _controlfp( 0, 0 );

	for ( int i = 0; i < flagsCount; i++ )
	{
		unsigned int bits = state & flags[i].mask;

		if ( flags[i].group )
			LogPrintf( PRN_ALL, "%s\n", flags[i].group );

		if ( bits & flags[i].value )
			LogPrintf( PRN_ALL, "%s\n", flags[i].name );
		else if ( !bits && !flags[i].value )
			LogPrintf( PRN_ALL, "%s\n", flags[i].name );
	}*/
	LogPrintf( PRN_ALL, "not implemented");
	LogPrintf( PRN_ALL, "=======================================\n" );
}

//---------------------------------------------------------
// nsCriticalSection::nsCriticalSection: 
//---------------------------------------------------------
nsCriticalSection::nsCriticalSection()
{
	assert(!"not implemented");
}

//---------------------------------------------------------
// nsCriticalSection::~nsCriticalSection: 
//---------------------------------------------------------
nsCriticalSection::~nsCriticalSection()
{

}

//---------------------------------------------------------
// nsCriticalSection::Enter: 
//---------------------------------------------------------
void nsCriticalSection::Enter()
{

}

//---------------------------------------------------------
// nsCriticalSection::Leave: 
//---------------------------------------------------------
void nsCriticalSection::Leave()
{

}


