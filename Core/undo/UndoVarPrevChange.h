// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoVarPrevChange.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "UndoRedoOperation.h"

template<typename TVar, typename TValue>
class nsUndoVarPrevChange final : public nsUndoRedoOperation {

public:
    explicit nsUndoVarPrevChange(TVar &var, TValue prevValue) : _var(var), _prevValue(prevValue) {
    }

    void Init() override {
        _newValue = _var;
        Redo();
    }

    void Redo() override {
        _var = _newValue;
    }

    void Undo() override {
        _var = _prevValue;
    }

private:
    TVar&     _var;
    TValue    _prevValue;
    TValue    _newValue;
};
