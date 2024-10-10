//
// Created by Roman on 8/2/2024.
//

#include "RayBoxTest.h"
#include "Engine/TimeFormat.h"
#include "Engine/RenAux.h"

nsRayBoxTest::nsRayBoxTest(float width, float height) :
        _width(width),
        _height(height),
        sdFunc(width, height) {
}

void nsRayBoxTest::GetLocalBounds(nsRect &bounds) {
    bounds.x = -_width / 2;
    bounds.y = -_height / 2;
    bounds.width = _width;
    bounds.height = _height;
}

void nsRayBoxTest::Loop() {
    origin.angle = origin.angle + rotationSpeed * g_frameTime;
}

void nsRayBoxTest::DrawContent(const nsVisualContext2d &context) {
    nsRect rect;
    GetLocalBounds(rect);

    nsSpriteDesc    desc;
    desc.size = nsVec2(_width, _height);
    desc.color = nsColor(0.2, 0.2, 0.2);
    desc.center = desc.size / 2;

    _device->StateApply(nullptr);
    _device->TextureBind(nullptr);
    desc.Draw(_device);
}

