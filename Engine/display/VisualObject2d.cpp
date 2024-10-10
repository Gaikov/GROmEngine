//
// Created by Roman on 6/27/2024.
//

#include "VisualObject2d.h"
#include "RenManager.h"
#include "Engine/display/context/VisualMaskManager.h"

int nsVisualObject2d::_leakCounter = 0;

nsVisualContainer2d *nsVisualObject2d::GetParent() const {
    return _parent;
}

nsVisualObject2d::nsVisualObject2d() : _device(nsRenDevice::Shared()->Device()) {
    _leakCounter ++;
}

nsVisualObject2d::~nsVisualObject2d() {
    _leakCounter --;
#ifdef DEBUG
    printf("visual 2d destroyed: %s\n", typeid(this).name());
#endif
}

void nsVisualObject2d::Destroy() {
    delete this;
}

void nsVisualObject2d::ApplyWorldMatrix() {
    nsMatrix m;
    origin.GetWorld().ToMatrix3(m);
    _device->LoadMatrix(m);
}

void nsVisualObject2d::DrawNode(const nsVisualContext2d &context) {
    auto mm = context.GetMaskManager();
    mm->PushMasks(&_masks, context);
    mm->EnsureMasks(context);

    ApplyWorldMatrix();
    DrawContent(context);

    mm->PopMasks(&_masks, context);
}

bool nsVisualObject2d::HitTest(float x, float y) {
    nsVec2 local = origin.ToLocal(nsVec2(x, y));
    nsRect rect;
    GetLocalBounds(rect);
    return rect.IsInside(local);
}

void nsVisualObject2d::GetBounds(nsRect &bounds, nsVisualObject2d *targetSpace) {
    GetLocalBounds(bounds);
    nsVec2 v[4] = {
            { bounds.minX(), bounds.minY() },
            { bounds.minX(), bounds.maxY() },
            { bounds.maxX(), bounds.minY() },
            { bounds.maxX(), bounds.maxY() },
    };

    for (auto & i : v) {
        i = origin.ToGlobal(i);
        if (targetSpace) {
            i = targetSpace->origin.ToLocal(i);
        }
    }

    float minX = v[0].x;
    float minY = v[0].y;
    float maxX = v[0].x;
    float maxY = v[0].y;

    for (int i = 1; i < 4; i++) {
        minX = std::min(minX, v[i].x);
        maxX = std::max(maxX, v[i].x);

        minY = std::min(minY, v[i].y);
        maxY = std::max(maxY, v[i].y);
    }

    bounds.x = minX;
    bounds.y = minY;
    bounds.width = maxX - minX;
    bounds.height = maxY - minY;
}

float nsVisualObject2d::GetWidth() {
    nsRect rect;
    GetLocalBounds(rect);
    return rect.width * origin.scale->x;
}

void nsVisualObject2d::SetWidth(float width) {
    nsRect rect;
    GetLocalBounds(rect);

    nsVec2 scale = origin.scale;
    if (rect.width != 0) {
        scale.x = width / rect.width;
        origin.scale = scale;
    }
}

float nsVisualObject2d::GetHeight() {
    nsRect rect;
    GetLocalBounds(rect);
    return rect.height * origin.scale->y;
}

void nsVisualObject2d::SetHeight(float height) {
    nsRect rect;
    GetLocalBounds(rect);

    nsVec2 scale = origin.scale;
    if (rect.height != 0) {
        scale.y = height / rect.height;
        origin.scale = scale;
    }
}

void nsVisualObject2d::LeaksCheck() {
#ifdef DEBUG
    if (_leakCounter != 0) {
        printf("Visual objects leaks: %i\n", _leakCounter);
    }
#endif
}

void nsVisualObject2d::AddMask(nsVisualMask *mask) {
#ifdef DEBUG
    auto it = std::find(_masks.begin(), _masks.end(),mask);
    assert(it == _masks.end());
#endif
    mask->visible = false;
    _masks.push_back(mask);
}

void nsVisualObject2d::RemoveMask(nsVisualMask *mask) {
    auto it = std::find(_masks.begin(), _masks.end(), mask);
    if (it != _masks.end()) {
        (*it)->visible = true;
        _masks.erase(it);
    }
}
