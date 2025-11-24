// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoPropertyChange.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "UndoRedoOperation.h"
#include "nsLib/models/Property.h"

template<typename TVar>
class nsUndoPropertyChange : public nsUndoRedoOperation {
public:
    explicit nsUndoPropertyChange(nsProperty<TVar> &var, TVar newValue) : _var(var), _newValue(newValue) {
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
    nsProperty<TVar> &_var;
    TVar            _prevValue;
    TVar            _newValue;
};
