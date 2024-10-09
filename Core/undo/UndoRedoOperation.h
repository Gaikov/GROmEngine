// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoRedoOperation.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Core/SmartPtr.h"

class nsUndoRedoOperation {
public:
    typedef nsSmartPtr<nsUndoRedoOperation> sp_t;

    virtual ~nsUndoRedoOperation() = default;

    virtual void Init() = 0;
    virtual void Redo() = 0;
    virtual void Undo() = 0;
};