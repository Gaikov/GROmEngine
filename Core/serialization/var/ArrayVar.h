// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ArrayVar.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <memory>

#include "Core/serialization/Serializable.h"
#include "nsLib/structs/Array.h"

class nsBaseArrayVar : public nsArray<std::shared_ptr<nsSerializable>>, public nsSerializable {
public:
    std::string serializationName;

    virtual nsSerializable* CreateItem() = 0;

    bool Serialize(nsScriptSaver &ss) override;
    bool Deserialize(script_state_t *ss) override;
    void ResetDefault() override;
};

template<typename TItem>
class nsArrayVar : public nsBaseArrayVar {
public:
    nsSerializable * CreateItem() override {
        return new TItem();
    };

    TItem* GetItem(const int index) {
        if ( index < 0 || index >= Size() ) return nullptr;
        auto &item = GetItems()[index];
        return dynamic_cast<TItem*>(item.get());
    }

    const TItem *GetItem(const int index) const {
        if ( index < 0 || index >= Size() ) return nullptr;
        const auto &item = GetItems()[index];
        return dynamic_cast<const TItem *>(item.get());
    }
};
