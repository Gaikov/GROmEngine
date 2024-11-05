//
// Created by Roman on 11/5/2024.
//

#include "CollisionTest.h"
#include "Engine/display/VisualSceneRender2d.h"
#include "Engine/utils/AppUtils.h"

bool nsCollisionTest::Init() {
    _stage = new nsVisualContainer2d();

    _rect = CreateRect(600, 300);
    _userRect = CreateRect(250, 100);

    return true;
}

void nsCollisionTest::Release() {
    _stage->Destroy();
}

void nsCollisionTest::Loop(float deltaTime) {
    auto size = nsAppUtils::GetClientSize();
    _rect->origin.pos = size / 2;
    float angle = _rect->origin.angle;
    angle += deltaTime;
    _rect->origin.angle = angle;
}

void nsCollisionTest::Draw() {
    nsVisualSceneRender2d::DrawScene(_stage);
}

nsVisualRect *nsCollisionTest::CreateRect(float width, float height) {
    auto rect = new nsVisualRect();
    rect->rect = {-width / 2, -height / 2, width, height};
    _stage->AddChild(rect);
    return rect;
}

bool nsCollisionTest::OnPointerMove(float x, float y, int pointerId) {
    BaseFunctionalTest::OnPointerMove(x, y, pointerId);

    auto size = nsAppUtils::GetClientSize();

    _userRect->origin.pos = {x, size.y - y};

    return true;
}
