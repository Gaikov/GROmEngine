// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoArrayAdd.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Core/undo/UndoRedoOperation.h"

template<typename TArray, typename TItem>
class nsUndoArrayAdd final : public nsUndoRedoOperation {
public:
    nsUndoArrayAdd(TArray &list, const TItem &item)
        : _list(list),
          _item(item),
          _index(-1) {
    }

    void Init() override {
        _index = _list.Size();
        Redo();
    }

    void Redo() override {
        _list.Insert(_index, _item);
    }

    void Undo() override {
        _list.RemoveAt(_index);
    }

private:
    TArray &_list;
    TItem _item;
    int _index;
};
