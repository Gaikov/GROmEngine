// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Env.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

class nsEnv {
public:
    virtual ~nsEnv() = default;
    virtual bool Init() = 0;
    virtual void MessagePopup(const char *title, const char *message) = 0;
    virtual void OpenUrl(const char *url) = 0;

    static bool Create();
    static void Destroy();
    static nsEnv* Shared() { return _shared; }

private:
    static nsEnv *_shared;
};