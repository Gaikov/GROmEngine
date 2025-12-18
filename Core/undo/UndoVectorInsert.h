// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoVectorInsert.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <assert.h>
#include <vector>

#include "UndoRedoOperation.h"

template<typename TItem>
class nsUndoVectorInsert : public nsUndoRedoOperation {
public:
    nsUndoVectorInsert(std::vector<TItem> &list, const int index, TItem &item) :
            _list(list), _index(index), _item(item) {
        assert(index < static_cast<int>(_list.size()));
    }

    void Init() override {
        Redo();
    }

    void Redo() override {
        _list.insert(_list.begin() + _index, _item);
    }

    void Undo() override {
        _list.erase(_list.begin() + _index);
    }

private:
    std::vector<TItem> &_list;
    int _index;
    TItem _item;
};