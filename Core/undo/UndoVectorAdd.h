// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoVectorAdd.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <algorithm>
#include <vector>

#include "UndoRedoOperation.h"

template<typename TItem>
class nsUndoVectorAdd : public nsUndoRedoOperation {
public:
    nsUndoVectorAdd(std::vector<TItem> &list, const TItem &item)
        : _list(list), _item(item) {

    }

    void Init() override {
        Redo();
    }

    void Redo() override {
        _list.push_back(_item);
    }

    void Undo() override {
        auto it = std::find(_list.begin(), _list.end(), _item);
        assert(it != _list.end());
        _list.erase(it);
    }

private:
    std::vector<TItem> &_list;
    TItem _item;
};
