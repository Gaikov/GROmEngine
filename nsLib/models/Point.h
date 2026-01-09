// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Point.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <memory>

#include "Property.h"
#include "nsLib/Vec2.h"

class nsPoint final : public nsProperty<nsVec2> {
    friend class nsParticlesEdgesSpawner;
public:
    typedef std::shared_ptr<nsPoint> sp_t;
    typedef std::function<void()> callback_t;

    class Owner {
    public:
        virtual ~Owner() = default;

    protected:
        friend class nsPoint;
        virtual void OnPointChanged(const nsPoint &point) = 0;
    };

    Owner *owner = nullptr;

    explicit nsPoint(const nsVec2 &value) : nsProperty(value) {
        AddHandler(nsPropChangedName::CHANGED, [this](const nsBaseEvent *e) {
            if (owner) {
                owner->OnPointChanged(*this);
            }
        });
    }

    nsPoint &operator = (const nsVec2 &value) {
        SetValue(value);
        return *this;
    }
};
