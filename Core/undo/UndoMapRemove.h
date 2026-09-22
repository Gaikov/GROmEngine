// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#pragma once

#include <cassert>
#include <map>

#include "UndoRedoOperation.h"

template<typename TKey, typename TValue>
class nsUndoMapRemove final : public nsUndoRedoOperation {
public:
    nsUndoMapRemove( std::map<TKey, TValue> &target, const TKey &key )
        : _map( target ), _key( key ) {
        assert( target.find( key ) != target.end() );
    }

    void Init() override {
        const auto it = _map.find( _key );
        assert( it != _map.end() );
        _value = it->second;
        Redo();
    }
    void Redo() override {
        const auto it = _map.find( _key );
        assert( it != _map.end() );
        _map.erase( it );
    }
    void Undo() override {
        const auto [it, inserted] = _map.emplace( _key, _value );
        (void)it;
        assert( inserted );
    }

private:
    std::map<TKey, TValue> &_map;
    TKey _key;
    TValue _value;
};
