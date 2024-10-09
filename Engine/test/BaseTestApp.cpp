//
// Created by Roman Gaikov on 2019-08-29.
//

#include "BaseTestApp.h"
#include "Core/Config.h"
#include "engine/RenManager.h"
#include "nsLib/log.h"
#include "EngineInfo.h"
#include "Input.h"

BaseTestApp::BaseTestApp() :
		_activeTest(nullptr),
		_lastTest(nullptr)
{

}

bool BaseTestApp::InitDialog()
{
	return true;
}

bool BaseTestApp::Init()
{
	_lastTest = g_cfg->RegVar("last_test", "", GVF_SAVABLE);
	g_cfg->RegCmd("test", [this](int argc, const char *argv[])
	{
		OnTestCommand(argc, argv);
	});

	RegisterTests();

	if (strlen(_lastTest->String()) > 0)
	{
		RunTest(_lastTest->String());
	}

    g_inp.ShowCursor(true);

	return true;
}

void BaseTestApp::Release()
{
    Log::Info("...releasing tests");
    if (_activeTest) {
        _activeTest->Release();
        _activeTest = nullptr;
    }

	for (auto &it : _tests)
	{
        Log::Info("deleting test: %s", it.first.c_str());
		delete it.second;
	}
}

void BaseTestApp::DrawWorld()
{
	g_renDev->ClearScene(CLR_CBUFF | CLR_ZBUFF);
	if (_activeTest)
	{
		_activeTest->Draw();
	}
}

void BaseTestApp::Loop(float frameTime)
{
	if (_activeTest)
	{
		_activeTest->Loop(frameTime);
	}
}

void BaseTestApp::OnActivate(bool active)
{

}

void BaseTestApp::OnPause(bool paused)
{

}

int BaseTestApp::GetWindowIcon()
{
	return 0;
}

IUserInput *BaseTestApp::GetUserInput()
{
	return _activeTest;
}

void BaseTestApp::GetGUIDimension(int &width, int &height)
{

}

void BaseTestApp::OnTestCommand(int argc, const char **argv)
{
	bool printTests = true;

	if (argc >= 2)
	{
		auto testName = argv[1];
		if (RunTest(testName))
		{
			_lastTest->SetString(testName);
			printTests = false;
		}
	}

	if (printTests)
	{
		LogPrintf(PRN_ALL, "Available tests: \n");
		for (auto &it : _tests)
		{
			LogPrintf(PRN_ALL, "%s\n", it.first.c_str());
		}
	}

}

bool BaseTestApp::RunTest(const char *name)
{
	LogPrintf(PRN_ALL, "===================================\n");
	LogPrintf(PRN_ALL, "test running: %s\n", name);
	LogPrintf(PRN_ALL, "===================================\n");
	auto it = _tests.find(name);
	if (it != _tests.end())
	{
		RunTest(it->second);
		return true;
	}
	else
	{
		LogPrintf(PRN_ALL, "WARNING: test not found: %s\n", name);
	}
	return false;
}


void BaseTestApp::RunTest(BaseFunctionalTest *test)
{
	if (_activeTest)
	{
		_activeTest->Release();
	}

	_activeTest = test;

	if (_activeTest)
	{
		if (!_activeTest->Init())
		{
			_activeTest->Release();
			_activeTest = nullptr;
		}
	}
}

void BaseTestApp::RegisterTest(const std::string &name, BaseFunctionalTest *test)
{
	_tests.insert(std::make_pair(name, test));
}

const char *BaseTestApp::GetVersionInfo()
{
	return StrPrintf("GROm Engine: %s", ENGINE_VERSION);
}



