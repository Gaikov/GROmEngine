// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoMapInsert.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <map>

#include "UndoRedoOperation.h"

template<typename TKey, typename TValue>
class nsUndoMapInsert : public nsUndoRedoOperation {
public:
    nsUndoMapInsert(std::map<TKey, TValue> &target, const TKey &key, const TValue &value)
        : _map(target), _item(value), _key(key) {
    }

    void Init() override {
        auto it = _map.find(_key);
        _hasValue = (it != _map.end());
        if (_hasValue) {
            _prevValue = it->second;
        }
        Redo();
    }

    void Redo() override {
        _map[_key] = _item;
    }

    void Undo() override {
        if (_hasValue) {
            _map[_key] = _prevValue;
        } else {
            _map.erase(_key);
        }
    }

private:
    std::map<TKey, TValue> &_map;
    TValue _item;
    bool _hasValue = false;
    TValue _prevValue;
    TKey _key;
};
