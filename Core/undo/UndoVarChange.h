// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoVarChange.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "UndoRedoOperation.h"

template<typename TVar>
class nsUndoVarChange : public nsUndoRedoOperation {

public:
    explicit nsUndoVarChange(TVar &var, TVar newValue) : _var(var), _newValue(newValue) {
    }

    void Init() override {
        _prevValue = _var;
        Redo();
    }

    void Redo() override {
        _var = _newValue;
    }

    void Undo() override {
        _var = _prevValue;
    }

private:
    TVar&   _var;
    TVar    _prevValue;
    TVar    _newValue;
};