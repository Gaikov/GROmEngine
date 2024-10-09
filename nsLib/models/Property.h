#pragma once
#include "nsLib/events/EventDispatcher.h"

struct nsPropChangedName final {
    enum {
        CHANGED = 0,
    };
};

template <typename TType> class nsPropChangedEvent : public nsBaseEvent {
public:
    TType prevValue;

    explicit nsPropChangedEvent(const TType &prevValue)
        : nsBaseEvent(nsPropChangedName::CHANGED),
          prevValue(prevValue) {
    }
};

template <typename TType>
class nsProperty : public nsEventDispatcher {
public:
    const nsProperty& operator = (const nsProperty &other) = delete;
    explicit nsProperty(const nsProperty& other) = delete;

    explicit nsProperty(const TType& value) {
        _value = value;
    }

    operator const TType& () const {
        return _value;
    }

    inline nsProperty& operator = (const TType& value) {
        if (_value != value) {
            nsPropChangedEvent<TType>    event(_value);
            _value = value;
            Emmit(event);
        }
        return *this;
    }

    const TType* operator->() const
    {
        return &_value;
    }

private:
    TType   _value;


};
