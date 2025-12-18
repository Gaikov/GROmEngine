// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoVectorRemove.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <assert.h>
#include <vector>

#include "UndoRedoOperation.h"

template<typename TItem>
class nsUndoVectorRemove : public nsUndoRedoOperation {
public:
    nsUndoVectorRemove(std::vector<TItem> &list, TItem &item) : _list(list), _item(item), _index(-1) {
    }

    void Init() override {
        for (int i = 0; i < static_cast<int>(_list.size()); ++i) {
            if (_list[i] == _item) {
                _index = i;
                break;
            }
        }
        assert(_index >= 0);

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
    TItem _item;
    int _index;
};



