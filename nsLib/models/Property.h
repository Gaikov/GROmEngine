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
    nsProperty& operator = (const nsProperty &other) {
        SetValue(other._value);
        return *this;
    };

    nsProperty(const nsProperty& other) = delete;

    nsProperty(const TType& value) {
        _defaultValue = value;
        _value = value;
    }

    operator const TType& () const {
        return _value;
    }

    inline nsProperty& operator = (const TType& value) {
        SetValue(value);
        return *this;
    }

    const TType* operator->() const
    {
        return &_value;
    }

    const TType& GetValue() {
        return _value;
    }

    void SetValue(const TType &value) {
        if (_value != value) {
            nsPropChangedEvent<TType>    event(_value);
            _value = value;
            Emmit(event);
        }
    }

    void Reset() {
        SetValue(_defaultValue);
    }

private:
    TType   _value;
    TType   _defaultValue;

};
