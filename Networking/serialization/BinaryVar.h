#pragma once
#include "nsLib/headers.h"

class IBinaryVar {
    friend class nsBinaryState;
public:
    virtual ~IBinaryVar() = default;
    [[nodiscard]] virtual size_t GetSize() const = 0;
    virtual void Serialize(void *buffer) = 0;
    virtual void Deserialize(const void *buffer) = 0;

private:
    IBinaryVar *next = nullptr;
};

template <typename TType>
class nsBinaryVar : public IBinaryVar {
public:
    typedef std::function<void()> onChanged;

    bool localLogic = false;

    nsBinaryVar(const TType &value) : _value(value) {}
    nsBinaryVar(const nsBinaryVar& other) = delete;

    [[nodiscard]] size_t GetSize() const override {
        return sizeof(TType);
    }

    void SetHandler(const onChanged& handler) {
        _handler = handler;
    }

    void BindVariable(TType &var) {
        SetHandler([this, &var] {
            var = _value;
        });
        var = _value;
    }

    void TriggerForced() const {
        if (_handler) {
            _handler();
        }
    }

    nsBinaryVar& operator = (const nsBinaryVar& other) {
        SetValue(other._value);
        return *this;
    }

    nsBinaryVar& operator = (const TType &value) {
        SetValue(value);
        return *this;
    }

    operator const TType& () const {
        return _value;
    }

    const TType& GetValue() {
        return _value;
    }

    void SetValue(const TType &value) {
        if (_value != value) {
            _value = value;
            TriggerForced();
        }
    }

    void Serialize(void *buffer) override {
        auto *data = static_cast<TType *>(buffer);
        *data = _value;
    }

    void Deserialize(const void *buffer) override {
        if (!localLogic) {
            auto *data = static_cast<const TType *>(buffer);
            SetValue(*data);
        }
    }

private:
    TType _value;
    onChanged _handler;
};


