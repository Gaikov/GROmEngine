// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file AbstractFactory.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/headers.h"
#include "nsLib/log.h"
#include "nsLib/StrTools.h"

template <class TBaseClass>
class nsAbstractFactory {
public:
    virtual ~nsAbstractFactory() = default;
    typedef std::function<TBaseClass*()>  builder_t;

    void RegisterBuilder(const char *name, const builder_t &builder) {
        if (HasBuilder(name)) {
            Log::Warning("overriding builder: %s", name);
        }
        _builders[name] = builder;
    }

    template <class TClass>
    void RegisterWithName() {
        RegisterBuilder(TClass::NAME,
                        []() -> TBaseClass* { return new TClass(); });
    }

    bool HasBuilder(const char *name) {
        return _builders.contains(name);
    }

    builder_t* GetBuilder(const char *name) {
        auto it = _builders.find(name);
        if (it != _builders.end()) {
            return &it->second;
        }
        Log::Warning("builder not found: %s", name);
        return nullptr;
    }

    virtual TBaseClass* Create(const char *name) {
        if (!StrCheck(name)) {
            Log::Warning("invalid builder name!");
            return nullptr;
        }

        auto builder = GetBuilder(name);
        if (!builder) {
            return nullptr;
        }

        return (*builder)();
    }

    void GetNamesList(std::vector<std::string> &list) {
        for (auto &builder : _builders) {
            list.push_back(builder.first);
        }
    }

protected:
    std::map<std::string, builder_t> _builders;
};