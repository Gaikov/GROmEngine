// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#pragma once

#include <cassert>

#include "Core/undo/UndoRedoOperation.h"
#include "nsLib/structs/Array.h"

template<typename TItem>
class nsUndoArrayInsert final : public nsUndoRedoOperation {
public:
    nsUndoArrayInsert( nsArray<TItem> &list, const int index, const TItem &item )
        : _list( list ), _item( item ), _index( index ) {
        assert( index >= 0 && index <= list.Size() );
    }

    void Init() override { Redo(); }
    void Redo() override {
        assert( _index >= 0 && _index <= _list.Size() );
        _list.Insert( _index, _item );
    }
    void Undo() override {
        assert( _index >= 0 && _index < _list.Size() );
        _list.RemoveAt( _index );
    }

private:
    nsArray<TItem> &_list;
    TItem _item;
    int _index;
};
