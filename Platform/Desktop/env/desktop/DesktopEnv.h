// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file DesktopEnv.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "env/Env.h"

class nsDesktopEnv : public nsEnv {
    bool Init() override;
    void MessagePopup(const char *title, const char *message) override;
    void OpenUrl(const char *url) override;
};
