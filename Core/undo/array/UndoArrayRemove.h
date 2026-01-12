// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoArrayRemove.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Core/undo/UndoRedoOperation.h"
#include "nsLib/structs/Array.h"

template<typename TItem>
class nsUndoArrayRemove final : public nsUndoRedoOperation {
public:
    nsUndoArrayRemove(nsArray<TItem> &list, const TItem &item) : _list(list), _item(item) {
    }

    void Init() override {
        _index = _list.GetIndex(_item);
        Redo();
    }
    void Redo() override {
        _list.RemoveAt(_index);
    }

    void Undo() override {
        _list.Insert(_index, _item);
    }

private:
    nsArray<TItem> &_list;
    TItem _item;
    int _index = -1;
};
