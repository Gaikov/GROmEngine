// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Locatable.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

class nsLocatable {
public:
    virtual ~nsLocatable() = default;
    virtual void OnCreated() = 0;
};