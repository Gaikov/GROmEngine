//
// Created by Roman on 8/8/2024.
//

#pragma once
#include "GroupLayout.h"
#include "Engine/display/align/Align.h"

class nsHGroupLayout : public nsGroupLayout {
public:
    nsProperty<float>   gap;
    nsProperty<nsAlign::Type>   horizontalAlign;

    nsHGroupLayout();
    const char * GetType() override { return nsVisualType::HGROUP; }

protected:
    void ComputeChildSize(nsVisualObject2d *child) override;

    void UpdateChildrenPositions() override;
    static bool HasHorizontalAnchors(nsVisualObject2d *child);

private:
    std::vector<nsRect> _boundsCache;
};
