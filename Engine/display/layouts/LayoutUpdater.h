// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file LayoutUtils.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "display/VisualObject2d.h"
#include "BaseLayout.h"

class nsLayoutUpdater final {
public:
    static void UpdateLayouts(nsVisualObject2d *sceneRoot);
    static void InvalidateLayout(nsVisualObject2d *child);
    static nsBaseLayout* FindParentLayout(nsVisualObject2d *child);

private:
    static void ComputeChildrenSize(nsVisualObject2d *obj);
    static void ValidateLayout(nsVisualObject2d *obj);
};