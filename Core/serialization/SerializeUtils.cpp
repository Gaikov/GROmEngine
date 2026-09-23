//
// Created by Roman on 12/22/2025.
//

#include "SerializeUtils.h"

#include <memory>

#include "Core/buffer/StringWriter.h"
#include "SerializableGroup.h"
#include "var/ArrayVar.h"

bool nsSerializeUtils::SerializeVar(nsScriptSaver &ss, const char *name, nsSerializable *var) {
    bool res = true;

    if (const auto array = dynamic_cast<nsBaseArrayVar*>(var)) {
        array->serializationName = name;
        res = array->Serialize(ss);
    }
    else if (dynamic_cast<nsSerializableGroup *>(var)) {
        if (ss.BlockBegin(name)) {
            res = var->Serialize(ss);
            ss.BlockEnd();
        }
    } else {
        ss.VarName(name);
        res = var->Serialize(ss);
    }
    return res;
}

bool nsSerializeUtils::DeserializeVar(script_state_t *ss, const char *name, nsSerializable *var) {
    bool res = true;
    if (const auto array = dynamic_cast<nsBaseArrayVar *>(var)) {
        array->serializationName = name;
        res = array->Deserialize(ss);
    }
    else if (dynamic_cast<nsSerializableGroup *>(var)) {
        if (ps_block_begin(ss, name)) {
            res = var->Deserialize(ss);
            ps_block_end(ss);
        }
    } else {
        if (ps_var_begin(ss, name)) {
            res = var->Deserialize(ss);
        }
    }
    return res;
}

bool nsSerializeUtils::SerializeToString(nsSerializable &model, std::string &result) {
    result.clear();
    auto writer = std::make_shared<nsStringWriter>();
    nsScriptSaver saver(writer);
    if (!model.Serialize(saver)) {
        return false;
    }
    result = writer->GetBuffer();
    return true;
}

bool nsSerializeUtils::Clone( nsSerializable &source, nsSerializable &destination ) {
    destination.ResetDefault();
    std::string markup;
    if ( !SerializeToString( source, markup ) ) return false;

    const auto state = ps_begin( markup.data() );
    if ( !state ) return false;
    const auto result = destination.Deserialize( state );
    ps_end( state );
    if ( !result ) destination.ResetDefault();
    return result;
}
