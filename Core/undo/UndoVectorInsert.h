// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoVectorInsert.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/headers.h"
#include "UndoRedoOperation.h"

template<typename TItem>
class nsUndoVectorInsert : public nsUndoRedoOperation {
public:
    nsUndoVectorInsert(std::vector<TItem> &list, int index, TItem item) :
            _list(list), _index(index), _item(item) {}

    void Init() override {
        Redo();
    }

    void Redo() override {
        _list.emplace_back();
        for (int i = _list.size() - 1; i > _index; i--) {
            _list[i] = _list[i - 1];
        }
        _list[_index] = _item;
    }

    void Undo() override {
        for (int i = _index; i < _list.size() - 1; i++) {
            _list[i] = _list[i + 1];
        }
        _list.pop_back();
    }

private:
    std::vector<TItem> &_list;
    int _index;
    TItem _item;
};