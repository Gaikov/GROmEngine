// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoVectorSet.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <vector>

#include "UndoRedoOperation.h"

template<typename TItem>
class nsUndoVectorSet : public nsUndoRedoOperation {
public:
    nsUndoVectorSet(std::vector<TItem> &list, int index, const TItem &newValue)
        : _list(list), _index(index), _newValue(newValue) {
    }

    void Init() override {
        _prevValue = _list[_index];
        Redo();
    }

    void Redo() override {
        _list[_index] = _newValue;
    }

    void Undo() override {
        _list[_index] = _prevValue;
    }

private:
    std::vector<TItem> &_list;
    int _index;
    TItem _prevValue;
    TItem _newValue;
};
