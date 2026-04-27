//
// Created by Roman Gaikov on 2019-08-29.
//

#include "BaseTestApp.h"
#include "Core/Config.h"
#include "Engine/RenManager.h"
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
		Log::Info("Available tests: ");
		for (auto &it : _tests)
		{
			Log::Info("%s", it.first.c_str());
		}
	}

}

bool BaseTestApp::RunTest(const char *name)
{
	Log::Info("===================================");
	Log::Info("test running: %s", name);
	Log::Info("===================================");
	auto it = _tests.find(name);
	if (it != _tests.end())
	{
		RunTest(it->second);
		return true;
	}
	else
	{
		Log::Warning("test not found: %s", name);
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



