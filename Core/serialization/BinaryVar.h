#pragma once
#include "nsLib/headers.h"

class IBinaryVar {
    friend class nsBinaryState;
public:
    virtual ~IBinaryVar() = default;
    [[nodiscard]] virtual size_t GetSize() const = 0;
    virtual void Serialize(void *buffer) = 0;
    virtual void Deserialize(const void *buffer) = 0

private:
    IBinaryVar *next = nullptr;
};

template <typename TType>
class nsBinaryVar : public IBinaryVar {
public:
    typedef std::function<void()> onChanged;

    explicit nsBinaryVar(const TType &value) : _value(value) {}

    [[nodiscard]] size_t GetSize() const override {
        return sizeof(_value);
    }

    void SetHandler(const onChanged& handler) {
        _handler = handler;
    }

    const TType& GetValue() {
        return _value;
    }

    void SetValue(const TType &value) {
        if (_value != value) {
            _value = value;
            if (_handler) {
                _handler();
            }
        }
    }

    void Serialize(void *buffer) override {
        auto *data = static_cast<TType *>(buffer);
        *data = _value;
    }

    void Deserialize(const void *buffer) override {
        auto *data = static_cast<const TType *>(buffer);
        SetValue(*data);
    }

private:
    TType _value;
    onChanged _handler;
};


