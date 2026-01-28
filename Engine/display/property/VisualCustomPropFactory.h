// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualCustomPropFactory.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "VisualCustomProp.h"
#include "Core/serialization/Serializable.h"
#include "nsLib/utils/NoDynamic.h"

class nsVisualCustomPropFactory final : public nsNoCopyable, public nsNoDynamic {
public:
    using builder_t = std::function<nsSerializable*()>;

    nsVisualCustomPropFactory();
    nsVisualCustomProp::sp_t Create(nsVisualCustomProp::Type type);

    void Serialize(nsScriptSaver &saver, const nsVisualCustomProp::sp_t &prop);
    nsVisualCustomProp::sp_t Deserialize(script_state_t *ss);

private:
    template<class TVar>
    void Register(const nsVisualCustomProp::Type type) {
        _builders[type] = [] {
            return new TVar();
        };
    }

    std::map<nsVisualCustomProp::Type, builder_t> _builders;
};
