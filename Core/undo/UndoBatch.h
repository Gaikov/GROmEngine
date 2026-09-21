// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoBatch.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "UndoRedoOperation.h"

class nsUndoBatch : public nsUndoRedoOperation {
public:
    void Add(std::unique_ptr<nsUndoRedoOperation> operation);
    void Add(nsUndoRedoOperation *operation);
    [[nodiscard]] bool IsEmpty() const { return _operations.empty(); }
    [[nodiscard]] std::size_t GetCount() const { return _operations.size(); }

    void Init() override;

    void Redo() override;

    void Undo() override;

private:
    std::vector<std::unique_ptr<nsUndoRedoOperation>> _operations;
};
