// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file AppUtils.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/Vec2.h"
#include "engine/Platform.h"

struct nsAppUtils final {
    static inline nsVec2 GetClientSize() {
        int w, h;
        App_GetPlatform()->GetClientSize(w, h);
        return { (float)w, (float)h };
    }
};