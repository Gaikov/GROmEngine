//
// Created by Roman on 12/22/2025.
//

#include "ArrayVar.h"

#include "serialization/SerializableGroup.h"

bool nsBaseArrayVar::Serialize(nsScriptSaver &ss) {
    for (auto &item : GetItems()) {
        const bool group = dynamic_cast<nsSerializableGroup*>(item.get());

        if (group) {
            ss.BlockBegin(serializationName.c_str());
        } else {
            ss.VarName(serializationName.c_str());
        }

        if (!item->Serialize(ss)) {
            return false;
        }

        if (group) {
            ss.BlockEnd();
        }
    }
    return true;
}

bool nsBaseArrayVar::Deserialize(script_state_t *ss) {
    bool hasNext = false;

    const std::shared_ptr<nsSerializable> test(CreateItem());
    const bool group = dynamic_cast<nsSerializableGroup*>(test.get());

    if (group) {
        ps_block_begin(ss, serializationName.c_str());
    } else {
        ps_var_begin(ss, serializationName.c_str());
    }

    bool res = true;
    do {
        std::shared_ptr<nsSerializable> item(CreateItem());

        if (!item->Deserialize(ss)) {
            res = false;
            break;
        }
        Add(item);

        if (group) {
            hasNext = ps_block_next(ss);
        } else {
            hasNext = ps_var_next(ss);
        }
    } while (hasNext);

    if (group) {
        ps_block_end(ss);
    }

    return res;
}

void nsBaseArrayVar::ResetDefault() {
    Clear();
}
