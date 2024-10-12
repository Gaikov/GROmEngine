// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file FontsCache.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/SubSystem.h"
#include "nsLib/factory/ResourcesCache.h"
#include "Engine/Font.h"

class nsFontsCache final : public nsSubSystem<nsFontsCache>, protected ResourcesCache<nsFont, int> {
public:
    nsFont* LoadFont(const char *fileName);

    inline nsFont* SysFont() {
        return _sysFont;
    }

protected:
    nsFont  *_sysFont;

    void OnRelease() override;
    nsFont *AllocateResource(const char *resourceName, int param) override;
    void FreeResource(nsFont *item) override;
    bool OnInit() override;
};