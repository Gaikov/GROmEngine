// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file engine.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "UserInput.h"

struct nsEngine
{
	static bool Init();
	static void Release(bool failed);
    static void OnActivateApp(bool active);
	static void MainLoop();

    static IUserInput* GetActiveInput();
    static void OnKeyDown(int keyCode, bool repeat, int mods);
    static void OnKeyUp(int keyCode, int mods);
    static void OnCharDown(char ch);

    static void OnPointerMove(const nsPointerData &pointer);
    static void OnPointerDown(const nsPointerData &pointer);
    static void OnPointerUp(const nsPointerData &pointer);
    static void OnPointerCancel(const nsPointerData &pointer);
};
