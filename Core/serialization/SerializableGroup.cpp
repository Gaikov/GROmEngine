//
// Created by Roman on 11/19/2025.
//

#include "SerializableGroup.h"

nsSerializableGroup::nsSerializableGroup(const char *name) : nsSerializable(name) {
}

bool nsSerializableGroup::Serialize(nsScriptSaver &ss) {
    if (ss.BlockBegin(GetName())) {
        for (const auto item : _items) {
            item->Serialize(ss);
        }
        ss.BlockEnd();
        return true;
    }
    return false;
}

bool nsSerializableGroup::Deserialize(script_state_t *ss) {
    if (ps_block_begin(ss, GetName())) {
        for (const auto item : _items) {
            item->Deserialize(ss);
        }
        ps_block_end(ss);
        return true;
    }
    return false;
}

void nsSerializableGroup::ResetDefault() {
    for (const auto item : _items) {
        item->ResetDefault();
    }
}
