//
// Created by Roman on 11/19/2025.
//

#include "SerializableGroup.h"

#include "nsLib/log.h"


void nsSerializableGroup::AddItem(const char *name, nsSerializable *item) {
    assert(!_items.contains(name));
    _items[name] = item;
}

bool nsSerializableGroup::Serialize(nsScriptSaver &ss) {
    for (const auto &item: _items) {
        bool res = true;
        const auto var = item.second;
        if (dynamic_cast<nsSerializableGroup *>(var)) {
            if (ss.BlockBegin(item.first.c_str())) {
                res = var->Serialize(ss);
                ss.BlockEnd();
            }
        } else {
            ss.VarName(item.first.c_str());
            res = var->Serialize(ss);
        }
        if (!res) return false;
    }
    return true;
}

bool nsSerializableGroup::Deserialize(script_state_t *ss) {
    for (const auto &item: _items) {
        const auto var = item.second;
        bool res = true;
        if (dynamic_cast<nsSerializableGroup *>(var)) {
            if (ps_block_begin(ss, item.first.c_str())) {
                res = var->Deserialize(ss);
                ps_block_end(ss);
            }
        } else {
            if (ps_var_begin(ss, item.first.c_str())) {
                res = var->Deserialize(ss);
            }
        }
        if (!res) return false;
    }
    return false;
}

void nsSerializableGroup::ResetDefault() {
    for (const auto &item: _items) {
        item.second->ResetDefault();
    }
}
