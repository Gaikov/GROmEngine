// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file NoDynamic.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

class nsNoDynamic {
public:
    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;
};