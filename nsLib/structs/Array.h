// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Vector.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <vector>

#include "nsLib/events/EventDispatcher.h"

class nsArrayEvent final : public nsBaseEvent {
public:
    static constexpr int ID = 0;

    explicit nsArrayEvent()
        : nsBaseEvent(ID) {
    }
};


template <typename TItem>
class nsArray : public nsEventDispatcher {
public:
    void Add(const TItem &item) {
        _items.push_back(item);
        Emmit(nsArrayEvent());
    }

    void Remove(const TItem &item) {
        _items.erase(std::remove(_items.begin(), _items.end(), item), _items.end());
        Emmit(nsArrayEvent());
    }

    void RemoveAt(int index) {
        _items.erase(_items.begin() + index);
        Emmit(nsArrayEvent());
    }

    int GetIndex(TItem &item) {
        return std::distance(_items.begin(), std::find(_items.begin(), _items.end(), item));
    }

    void Insert(int index, const TItem &item) {
        _items.insert(_items.begin() + index, item);
        Emmit(nsArrayEvent());
    }

    void Clear() {
        _items.clear();
        Emmit(nsArrayEvent());
    }

    int Size() {
        return _items.size();
    }

    TItem &operator[](int index) { return _items[index]; }

    const std::vector<TItem> &GetItems() const { return _items; }

private:
    std::vector<TItem> _items;
};
