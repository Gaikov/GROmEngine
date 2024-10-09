//
// Created by Roman on 8/1/2024.
//

#include "LayoutUpdater.h"
#include "BaseLayout.h"

void nsLayoutUpdater::InvalidateLayout(nsVisualObject2d *child) {
    auto parent = FindParentLayout(child);
    if (parent) {
        parent->InvalidateLayout();
    }
}

nsBaseLayout *nsLayoutUpdater::FindParentLayout(nsVisualObject2d *child) {
    auto parent = child;
    while (parent) {
        auto layout = dynamic_cast<nsBaseLayout*>(parent);
        if (layout) {
            return layout;
        }
        parent = parent->GetParent();
    }
    return nullptr;
}

void nsLayoutUpdater::UpdateLayouts(nsVisualObject2d *sceneRoot) {
    ComputeChildrenSize(sceneRoot);
    ValidateLayout(sceneRoot);
}

void nsLayoutUpdater::ComputeChildrenSize(nsVisualObject2d *obj) {
    auto layout = dynamic_cast<nsBaseLayout*>(obj);
    if (layout) {
        layout->ComputeChildrenSize();
    }

    auto container = dynamic_cast<nsVisualContainer2d*>(obj);
    if (container) {
        for (auto child : container->GetChildren()) {
            ComputeChildrenSize(child);
        }
    }
}

void nsLayoutUpdater::ValidateLayout(nsVisualObject2d *obj) {
    auto container = dynamic_cast<nsVisualContainer2d*>(obj);
    if (container) {
        for (auto child : container->GetChildren()) {
            ValidateLayout(child);
        }
    }

    auto layout = dynamic_cast<nsBaseLayout*>(obj);
    if (layout) {
        layout->ValidateLayout();
    }
}



