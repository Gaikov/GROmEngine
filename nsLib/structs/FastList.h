//
// Created by Roman on 7/26/2024.
//

#pragma once

template <class TItem> class nsFastList;

template <class TItem>
class nsListItem {
friend class nsFastList<TItem>;

public:
    TItem *NextItem() {
        return _next;
    }

    TItem *PrevItem() {
        return _prev;
    }

private:
    TItem *_prev = nullptr;
    TItem *_next = nullptr;
};

template <class TItem>
class nsFastList {
public:
    nsFastList() = default;

    void Add(TItem *item) {
        if (_head) {
            _head->_prev = item;
        }
        item->_prev = nullptr;
        item->_next = _head;
        _head = item;
    }

    void Remove(TItem *item) {
        if (item == _head) {
            _head = _head->_next;
        } else {
            TItem *prev = item->_prev;
            TItem *next = item->_next;
            if (prev) {
                prev->_next = next;
            }
            if (next) {
                next->_prev = prev;
            }
        }

        item->_prev = nullptr;
        item->_next = nullptr;
    }

    /*void InsertAt(int index, TItem *item) {
        if (!index) {
            Add(item);
        } else {
            for (auto atItem = _head; atItem && index; atItem->_next, index--) {

            }
        }
    }*/

    bool IsEmpty() {
        return !_head;
    }

    TItem* GetHead() {
        return _head;
    }

    int ComputeAmount() {
        int count = 0;
        for (auto i = _head; i; i = i->_next) {
            count++;
        }
        return count;
    }

    bool HasItem(TItem *item) {
        for (auto i = _head; i; i = i->_next) {
            if (i == item) {
                return true;
            }
        }
        return false;
    }

private:
    TItem *_head = nullptr;
};