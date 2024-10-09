// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file LayoutAnchor.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/models/Property.h"

class nsLayoutAnchor final {
public:
    enum nsAnchorType {
        NONE,
        NUMBER,
        PERCENT
    };

    nsProperty<nsAnchorType> type;
    nsProperty<float> value;

    void Set(nsAnchorType anchorType, float anchorValue);
    void SetNumber(float n);

public:
    nsLayoutAnchor();
    bool IsUsed() const;
    float Compute(float size) const;
};