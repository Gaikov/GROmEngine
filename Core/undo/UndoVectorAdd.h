// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoVectorAdd.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <cassert>
#include <vector>

#include "UndoRedoOperation.h"

template<typename TItem>
class nsUndoVectorAdd : public nsUndoRedoOperation {
public:
    nsUndoVectorAdd(std::vector<TItem> &list, const TItem &item)
        : _list(list), _item(item) {

    }

    void Init() override {
        _index = static_cast<int>(_list.size());
        Redo();
    }

    void Redo() override {
        assert(_index >= 0 && _index <= static_cast<int>(_list.size()));
        _list.push_back(_item);
    }

    void Undo() override {
        assert(_index >= 0 && _index < static_cast<int>(_list.size()));
        _list.erase(_list.begin() + _index);
    }

private:
    std::vector<TItem> &_list;
    int _index = -1;
    TItem _item;
};
