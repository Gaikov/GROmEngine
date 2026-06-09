// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file FilePathVar.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "SerializableVar.h"
#include "nsLib/FilePath.h"

class nsFilePathVar : public nsSerializableVar<std::string> {
public:
    nsFilePathVar() : nsSerializableVar("") {
    }

    nsFilePathVar(const std::string &defValue)
        : nsSerializableVar(defValue) {
    }

    nsFilePathVar(const char *defValue)
        : nsSerializableVar(defValue) {
    }

    nsFilePathVar &operator=(const char *value) {
        SetValue(value);
        return *this;
    }

    nsFilePathVar &operator=(const std::string &value) {
        SetValue(value);
        return *this;
    }

    bool operator==(const char *value) const {
        return GetValue() == value;
    }

    bool operator==(const std::string &value) const {
        return GetValue() == value;
    }

    void SetBaseFolder(const nsFilePath &folder) {
        _baseFolder = folder;
        const auto &value = GetValue();
        if (!value.empty() && !nsFilePath::IsAbsolute(value.c_str())) {
            SetValue(_baseFolder.ResolvePath(value.c_str()).AsChar());
        }
    }

    const nsFilePath &GetBaseFolder() const {
        return _baseFolder;
    }

    bool Serialize(nsScriptSaver &ss) override;
    bool Deserialize(script_state_t *ss) override;

private:
    nsFilePath _baseFolder = "";
};
