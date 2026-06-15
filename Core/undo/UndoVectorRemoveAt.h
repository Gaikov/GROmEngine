// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoVectorRemoveAt.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <vector>

#include "UndoRedoOperation.h"

template<typename TItem>
class nsUndoVectorRemoveAt : public nsUndoRedoOperation {
public:
    nsUndoVectorRemoveAt(std::vector<TItem> &list, int index)
        : _list(list), _index(index), _item(list[index]) {
    }

    void Init() override {
        Redo();
    }

    void Redo() override {
        _list.erase(_list.begin() + _index);
    }

    void Undo() override {
        _list.insert(_list.begin() + _index, _item);
    }

private:
    std::vector<TItem> &_list;
    int _index;
    TItem _item;
};
