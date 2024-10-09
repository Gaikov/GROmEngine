// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VGroupLayout.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "GroupLayout.h"
#include "engine/display/align/Align.h"

class nsVGroupLayout : public nsGroupLayout {
public:
    nsProperty<float>           gap;
    nsProperty<nsAlign::Type>   verticalAlign;

    nsVGroupLayout();

protected:
    void ComputeChildSize(nsVisualObject2d *child) override;

    void UpdateChildrenPositions() override;
    static bool HasVerticalAnchors(nsVisualObject2d *child);

private:
    std::vector<nsRect> _boundsCache;
};
