// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SerializableGroup.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Serializable.h"

class nsSerializableGroup : public nsSerializable {
public:
    explicit nsSerializableGroup(const char *name);
    void AddItem(nsSerializable *item) { _items.push_back(item); }
    bool Serialize(nsScriptSaver &ss) override;
    bool Deserialize(script_state_t *ss) override;
    void ResetDefault() override;

private:
    std::vector<nsSerializable *> _items;
};
