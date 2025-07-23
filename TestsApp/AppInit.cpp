// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file AppInit.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "Core/AppInfo.h"

class nsTestApp : public IAppInfo
{
	const char *GetAppName() override
	{ return "test_app"; }

	const char *GetDefCfgPath() override
	{ return nullptr; }
};

static nsTestApp app;

IAppInfo *App_GetInfo()
{
	return &app;
}

