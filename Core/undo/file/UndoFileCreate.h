// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoFileCreate.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Core/undo/UndoRedoOperation.h"
#include "nsLib/FilePath.h"

class nsUndoFileCreate : public nsUndoRedoOperation {
public:
    explicit nsUndoFileCreate(const nsFilePath &path, const char* content);

    void Init() override;
    void Redo() override;
    void Undo() override;

private:
    nsFilePath _path;
    std::string _content;
};
