//
// Created by Raman Haikou on 2019-07-24.
//

#include "Core/AppInfo.h"
#include "engine/Platform.h"
#include "UnitCommon.h"

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

class nsCoreUnit : public IAppInfo
{
public:
	const char *GetAppName() override
	{
		return "core_unit";
	}

	const char *GetDefCfgPath() override
	{
		return "core_unit.cfg";
	}
};

IAppInfo *App_GetInfo()
{
	static nsCoreUnit *app = nullptr;
	if (!app) {
		app = new nsCoreUnit();
	}
	return app;
}

Platform* App_GetPlatform() {
    return nullptr;
}

