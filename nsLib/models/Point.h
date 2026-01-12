// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Point.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <memory>

#include "nsLib/Vec2.h"

class nsPoint final {
    friend class nsParticlesEdgesSpawner;
public:
    typedef std::shared_ptr<nsPoint> sp_t;

    class Owner {
    public:
        virtual ~Owner() = default;

    protected:
        friend class nsPoint;
        virtual void OnPointChanged(const nsPoint &point) = 0;
    };

    Owner *owner = nullptr;

    explicit nsPoint(const nsVec2 &value) : _value(value) {
    }

    [[nodiscard]] const nsVec2 &GetValue() const { return _value; }

    operator const nsVec2& () const { return _value; }

    nsPoint &operator = (const nsVec2 &value) {
        if (_value != value) {
            _value = value;
            if (owner) {
                owner->OnPointChanged(*this);
            }
        }
        return *this;
    }

private:
    nsVec2 _value;

};
