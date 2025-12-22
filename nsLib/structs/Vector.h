// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Vector.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <vector>

#include "nsLib/events/EventDispatcher.h"

class nsVectorEvent final : public nsBaseEvent {
public:
    static constexpr int ID = 0;

    explicit nsVectorEvent()
        : nsBaseEvent(ID) {
    }
};


template <typename TItem>
class nsVector : public nsEventDispatcher {
public:
    void Add(const TItem &item) {
        _items.push_back(item);
        Emmit(nsVectorEvent());
    }

    void Remove(TItem &item) {
        _items.erase(std::remove(_items.begin(), _items.end(), item), _items.end());
        Emmit(nsVectorEvent());
    }

    void Clear() {
        _items.clear();
        Emmit(nsVectorEvent());
    }

    const std::vector<TItem> &GetItems() const { return _items; }

private:
    std::vector<TItem> _items;
};
