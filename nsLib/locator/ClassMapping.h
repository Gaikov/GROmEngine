// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ClassMapping.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/headers.h"
#include "Locatable.h"

class nsClassMapping {
public:
    template<class TClass> nsClassMapping& To() {
        assert(!_instance);
        _factoryFunc = []() -> nsLocatable* {
            return new TClass();
        };
        return *this;
    }

    nsClassMapping& AsSingleton();
    nsLocatable* GetInstance();

private:
    bool        _singleton = false;
    nsLocatable *_instance = nullptr;
    std::function<nsLocatable*()>   _factoryFunc;
};