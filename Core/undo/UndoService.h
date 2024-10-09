// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoService.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/SubSystem.h"
#include "nsLib/headers.h"
#include "UndoRedoOperation.h"

class nsUndoService : public nsSubSystem<nsUndoService> {
protected:
    void OnRelease() override;

public:
    void Push(nsUndoRedoOperation *op);
    void Undo();
    void Redo();
    void Clear();

private:
    typedef std::vector<nsUndoRedoOperation *> opsList_t;

private:
    opsList_t _doneList;
    opsList_t _unDoneList;

private:
    void ClearUnDone();
};