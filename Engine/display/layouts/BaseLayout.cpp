//
// Created by Roman on 7/31/2024.
//

#include "BaseLayout.h"
#include "RenAux.h"
#include "nsLib/log.h"

nsBaseLayout::nsBaseLayout() :
        boundsType(RECTANGLE),
        _onChangeHandler([this](const nsBaseEvent &) -> void {
            InvalidateLayout();
        }) {

    boundsType.AddHandler(nsPropChangedName::CHANGED, _onChangeHandler);
}

void nsBaseLayout::InvalidateLayout() {
    nsVisualContainer2d *parent = this;
    while (parent) {
        auto layout = dynamic_cast<nsBaseLayout *>(parent);
        if (layout) {
            layout->_validLayout = false;
        }
        parent = parent->GetParent();
    }
}

void nsBaseLayout::ValidateLayout() {
    if (_validLayout) {
        return;
    }
    UpdateChildrenPositions();
    _validLayout = true;
    Log::Info("Layout updated");
}

void nsBaseLayout::ComputeChildrenSize() {
    for (auto child: GetChildren()) {
        ComputeChildSize(child);
    }
}

void nsBaseLayout::SetContentWidth(float value) {
    if (_contentWidth != value) {
        _contentWidth = value;
        InvalidateLayout();
    }
}

void nsBaseLayout::SetContentHeight(float value) {
    if (_contentHeight != value) {
        _contentHeight = value;
        InvalidateLayout();
    }
}

void nsBaseLayout::DrawContent(const nsVisualContext2d &context) {
    nsVisualContainer2d::DrawContent(context);
    if (drawFrame) {
        ApplyWorldMatrix();
        nsRect rect = {
                0, 0, _contentWidth, _contentHeight
        };
        nsGizmos::DrawRect(rect, nsColor::white);
    }
}

void nsBaseLayout::UpdateChildrenPositions() {
    for (auto child: GetChildren()) {
        ComputeChildPlacement(child);
    }
}

void nsBaseLayout::SetWidth(float width) {
    float scale = origin.scale->x;
    if (scale == 0) {
        scale = 1;
    }
    SetContentWidth(width / scale);
}

void nsBaseLayout::SetHeight(float height) {
    float scale = origin.scale->y;
    if (scale == 0) {
        scale = 1;
    }
    SetContentHeight(height / scale);
}

void nsBaseLayout::DestroyChildren() {
    nsVisualContainer2d::DestroyChildren();
    InvalidateLayout();
}

void nsBaseLayout::AddChild(nsVisualObject2d *obj) {
    nsVisualContainer2d::AddChild(obj);
    InvalidateLayout();
}

void nsBaseLayout::RemoveChild(nsVisualObject2d *obj) {
    nsVisualContainer2d::RemoveChild(obj);
    InvalidateLayout();
}

void nsBaseLayout::RemoveChildren() {
    nsVisualContainer2d::RemoveChildren();
    InvalidateLayout();
}

void nsBaseLayout::GetLocalBounds(nsRect &bounds) {
    if (boundsType == MIXED) {
        nsVisualContainer2d::GetLocalBounds(bounds);
        nsRect rect = {0, 0, _contentWidth, _contentHeight};
        bounds += rect;
    } else if (boundsType == CHILDREN) {
        nsVisualContainer2d::GetLocalBounds(bounds);
    } else {
        bounds = {0, 0, _contentWidth, _contentHeight};
    }
}


