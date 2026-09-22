// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#pragma once

#include <cassert>

#include "Core/undo/UndoRedoOperation.h"
#include "nsLib/structs/Array.h"

template<typename TItem>
class nsUndoArraySet final : public nsUndoRedoOperation {
public:
    nsUndoArraySet( nsArray<TItem> &list, const int index, const TItem &value )
        : _list( list ), _value( value ), _index( index ) {
        assert( index >= 0 && index < list.Size() );
    }

    void Init() override {
        _previous = _list[_index];
        Redo();
    }
    void Redo() override {
        assert( _index >= 0 && _index < _list.Size() );
        _list.Set( _index, _value );
    }
    void Undo() override {
        assert( _index >= 0 && _index < _list.Size() );
        _list.Set( _index, _previous );
    }

private:
    nsArray<TItem> &_list;
    TItem _previous;
    TItem _value;
    int _index;
};
