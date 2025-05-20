// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file nsAutoDestroy.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "nsLib/headers.h"

class nsAutoDestroy final {
public:
    typedef std::function<void()> destroyFunc_t;

    ~nsAutoDestroy();

    void AddDestroyCallback( const destroyFunc_t &func );

private:
    std::vector<destroyFunc_t> _destroyFuncs;
};
