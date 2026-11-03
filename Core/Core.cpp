// Copyright (c) 2005-2007, Gaikov Roman. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Core.cpp
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#include "Core.h"
#include "sys.h"
#include "nsLib/log.h"
#include "Package.h"
#include "Config.h"
#include "Name.h"
#include "debug/LogFile.h"
#include "debug/LogStdOut.h"
#include "memory/FastMemManager.h"
#include "Engine/Platform.h"

//---------------------------------------------------------
// nsCore::Init: 
//---------------------------------------------------------
bool nsCore::Init()
{
	if (!App_GetInfo()->ConfigurePlatform(*App_GetPlatform())) return false;
	Log::Init();
	Log::Shared()->AddPolicy(new LogStdOut());
	const auto logPath = App_GetPlatform()->GetInternalPath(
        StrPrintf("%s.log", App_GetInfo()->GetAppName()));
	if (!logPath.IsEmpty()) logPath.CreateParentFolders();
	g_log.Open(logPath.AsChar());
	Log::Info("*** Core Init ***");

	Log::Info("OS Installed: %s", Sys_OSName().c_str());
	Log::Info("CPU Speed: %.2f", Sys_CPUSpeed());

	if (!nsFastMemManager::Init()) {
		return false;
	}

	App_GetInfo()->ConfigurePackage(g_pack);
	if (!g_pack.Init()) return false;
    if (!App_GetInfo()->Init()) {
        return false;
    }
	if (!nsConfig::Init()) return false;

	Log::Info("*** Core Started ***");
	return true;
}

//---------------------------------------------------------
// nsCore::Release: 
//---------------------------------------------------------
void nsCore::Release()
{
	Log::Info("*** Core Shutdown ***");

	nsConfig::Release();
	App_GetInfo()->Release();
	g_pack.Release();

	nsNamePool::Free();

	nsFastMemManager::Release();

	g_log.Close();
	Log::Info("");
	Log::Release();
}
