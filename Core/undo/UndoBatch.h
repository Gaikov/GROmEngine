// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoBatch.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/headers.h"
#include "UndoRedoOperation.h"

class nsUndoBatch : public nsUndoRedoOperation {
public:
    ~nsUndoBatch() override;

    void Add(nsUndoRedoOperation *op);

    void Init() override;

    void Redo() override;

    void Undo() override;

private:
    std::vector<nsUndoRedoOperation*> _list;
};