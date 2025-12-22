//
// Created by Roman on 11/19/2025.
//

#include "SerializableFile.h"

#include "ParseFile.h"
#include "nsLib/log.h"

bool nsSerializableFile::Load(const char *fileName) {
    Log::Info("Loading model: %s", fileName);
    ResetDefault();
    nsParseFile pf;
    if (const auto ss = pf.BeginFile(fileName)) {
        Deserialize(ss);
        return true;
    }
    return false;
}

bool nsSerializableFile::Save(const char *fileName) {
    Log::Info("Saving model: %s", fileName);
    nsScriptSaver   ss(fileName);
    if (ss.IsValid()) {
        return Serialize(ss);
    }
    return false;
}
