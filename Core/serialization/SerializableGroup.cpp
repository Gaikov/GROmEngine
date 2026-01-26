//
// Created by Roman on 11/19/2025.
//

#include "SerializableGroup.h"

#include "SerializeUtils.h"
#include "nsLib/log.h"


void nsSerializableGroup::AddItem(const char *name, nsSerializable *item) {
    assert(!_items.contains(name));
    _items[name] = item;
}

bool nsSerializableGroup::Serialize(nsScriptSaver &ss) {
    for (const auto &item: _items) {
        if (!nsSerializeUtils::SerializeVar(ss, item.first.c_str(), item.second)) {
            return false;
        }
    }
    return true;
}

bool nsSerializableGroup::Deserialize(script_state_t *ss) {
    for (const auto &item: _items) {
        const auto var = item.second;
        if (!nsSerializeUtils::DeserializeVar(ss, item.first.c_str(), var)) {
            return false;
        }
    }
    return true;
}

void nsSerializableGroup::ResetDefault() {
    for (const auto &item: _items) {
        item.second->ResetDefault();
    }
}