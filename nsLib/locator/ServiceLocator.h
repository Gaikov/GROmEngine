// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ServiceLocator.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/SubSystem.h"
#include "ClassMapping.h"
#include "nsLib/log.h"

class nsServiceLocator : public nsSubSystem<nsServiceLocator> {
public:
    template <class TClass> nsClassMapping& MapClass() {
        auto name = typeid(TClass).name();
        auto &mapping = _mapping[name];
        mapping.To<TClass>();
        return mapping;
    }

    template<class TClass> TClass* Locate() {
        auto name = typeid(TClass).name();
        auto it = _mapping.find(name);
        if (it != _mapping.end()) {
            auto instance= it->second.GetInstance();
            auto res = dynamic_cast<TClass*>(instance);
            if (res) {
                return res;
            }
            Log::Warning("Invalid inheritance: %s -> %s", name, typeid(instance).name());
        }

        Log::Warning("Can't locate object: %s", name);
        return nullptr;
    }

private:
    std::map<std::string, nsClassMapping>   _mapping;
};

template <class TClass> TClass* Locate() {
    return nsServiceLocator::Shared()->Locate<TClass>();
}