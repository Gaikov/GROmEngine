//
// Created by Roman Gaikov on 2019-08-29.
//

#pragma once
#include "Core/headers.h"
#include "Core/Var.h"
#include "BaseFunctionalTest.h"
#include "Engine/GameApp.h"

class BaseTestApp : public IGameApp
{
protected:
	BaseTestApp();

	virtual void RegisterTests() = 0;

	void RegisterTest(const std::string &name, BaseFunctionalTest *test);

	bool InitDialog() override;

	bool Init() override;

	void Release() override;

	void DrawWorld() override;

	void Loop(float frameTime) override;

	void OnActivate(bool active) override;

	void OnPause(bool paused) override;

	int GetWindowIcon() override;

	IUserInput *GetUserInput() override;

	void GetGUIDimension(int &width, int &height) override;

	const char *GetVersionInfo() override;

private:

	std::map<std::string, BaseFunctionalTest *> _tests;
	BaseFunctionalTest                          *_activeTest;
	nsVar                                       *_lastTest;

	void OnTestCommand(int argc, const char *argv[]);

	void RunTest(BaseFunctionalTest *test);

	bool RunTest(const char *name);
};