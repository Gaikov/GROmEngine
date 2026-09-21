// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoVectorSet.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <cassert>
#include <vector>

#include "UndoRedoOperation.h"

template<typename TItem>
class nsUndoVectorSet : public nsUndoRedoOperation {
public:
    nsUndoVectorSet(std::vector<TItem> &list, int index, const TItem &newValue)
        : _list(list), _index(index), _newValue(newValue) {
        assert(index >= 0 && index < static_cast<int>(_list.size()));
    }

    void Init() override {
        assert(_index >= 0 && _index < static_cast<int>(_list.size()));
        _prevValue = _list[_index];
        Redo();
    }

    void Redo() override {
        assert(_index >= 0 && _index < static_cast<int>(_list.size()));
        _list[_index] = _newValue;
    }

    void Undo() override {
        assert(_index >= 0 && _index < static_cast<int>(_list.size()));
        _list[_index] = _prevValue;
    }

private:
    std::vector<TItem> &_list;
    int _index;
    TItem _prevValue;
    TItem _newValue;
};
