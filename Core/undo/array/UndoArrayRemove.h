// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoArrayRemove.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <cassert>
#include "Core/undo/UndoRedoOperation.h"
#include "nsLib/structs/Array.h"

template<typename TItem>
class nsUndoArrayRemove final : public nsUndoRedoOperation {
public:
    nsUndoArrayRemove(nsArray<TItem> &list, const TItem &item) : _list(list), _item(item) {
    }

    void Init() override {
        _index = _list.GetIndex(_item);
        assert(_index >= 0 && _index < _list.Size());
        Redo();
    }
    void Redo() override {
        assert(_index >= 0 && _index < _list.Size());
        _list.RemoveAt(_index);
    }

    void Undo() override {
        assert(_index >= 0 && _index <= _list.Size());
        _list.Insert(_index, _item);
    }

private:
    nsArray<TItem> &_list;
    TItem _item;
    int _index = -1;
};
