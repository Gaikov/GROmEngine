// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoArrayAdd.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Core/undo/UndoRedoOperation.h"
#include "nsLib/structs/Array.h"

template<typename TItem>
class nsUndoArrayAdd : public nsUndoRedoOperation {
public:
    nsUndoArrayAdd(nsArray<TItem> &list, const TItem &item)
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
    nsArray<TItem> &_list;
    TItem _item;
    int _index;
};
