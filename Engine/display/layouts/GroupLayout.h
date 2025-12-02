// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualGroup2d.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "BaseLayout.h"
#include "Engine/display/VisualType.h"

class nsGroupLayout : public nsBaseLayout {
protected:
    const char * GetType() override { return nsVisualType::GROUP; }
    void ComputeChildSize(nsVisualObject2d *child) override;
    void ComputeChildPlacement(nsVisualObject2d *child) override;
    void ComputeChildXPlacement(nsVisualObject2d *child, const nsRect &bounds) const;
    void ComputeChildYPlacement(nsVisualObject2d *child, const nsRect &bounds) const;
    void ComputeChildWidth(nsVisualObject2d *child) const;
    void ComputeChildHeight(nsVisualObject2d *child) const;
};
