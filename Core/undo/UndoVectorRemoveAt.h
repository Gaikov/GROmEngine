// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoVectorRemoveAt.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <cassert>
#include <vector>

#include "UndoRedoOperation.h"

template<typename TItem>
class nsUndoVectorRemoveAt : public nsUndoRedoOperation {
public:
    nsUndoVectorRemoveAt(std::vector<TItem> &list, int index)
        : _list(list), _index(index) {
        assert(index >= 0 && index < static_cast<int>(_list.size()));
    }

    void Init() override {
        assert(_index >= 0 && _index < static_cast<int>(_list.size()));
        _item = _list[_index];
        Redo();
    }

    void Redo() override {
        assert(_index >= 0 && _index < static_cast<int>(_list.size()));
        _list.erase(_list.begin() + _index);
    }

    void Undo() override {
        assert(_index >= 0 && _index <= static_cast<int>(_list.size()));
        _list.insert(_list.begin() + _index, _item);
    }

private:
    std::vector<TItem> &_list;
    int _index;
    TItem _item;
};
