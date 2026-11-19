// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#pragma once

#include "nsLib/math/Rect.h"

class nsScissorRect final {
public:
    // Logical top-left pixels of the active render target. Empty means no drawing.
    static nsRect Normalize(const nsRect &rect, int targetWidth, int targetHeight);
};
