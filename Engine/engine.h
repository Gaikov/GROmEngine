// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file engine.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_engine_H_
#define	_engine_H_

#include "UserInput.h"
#include "nsLib/math/ortho/OrthogonalNativeView.h"

struct nsEngine
{
	static bool Init();
	static void Release(bool failed);
    static void OnActivateApp(bool active);
	static void MainLoop();

    static IUserInput* GetActiveInput();
    static void OnKeyDown(int keyCode, bool repeat);
    static void OnKeyUp(int keyCode);
    static void OnCharDown(char ch);

    static void OnPointerMove(int pointerId, int clientX, int clientY);
    static void OnPointerDown(int pointerId, int clientX, int clientY);
    static void OnPointerUp(int pointerId, int clientX, int clientY);
};

#endif //_engine_H_