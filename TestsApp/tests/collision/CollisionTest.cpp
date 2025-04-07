//
// Created by Roman on 11/5/2024.
//

#include "CollisionTest.h"
#include "Engine/display/VisualSceneRender2d.h"
#include "Engine/utils/AppUtils.h"
#include "nsLib/math/collision/CollisionMath.h"
#include "Engine/display/graphics/VisualCircle.h"
#include "Engine/KeyCodes.h"

nsCollisionTest::nsCollisionTest() : _rectColliding(false) {
    _rectColliding.AddHandler(nsPropChangedName::CHANGED, [this](const nsBaseEvent *e) {
        _userRect->visible = _rectColliding;
        _userCircle->visible = !_rectColliding;
    });
}

bool nsCollisionTest::Init() {
    _stage = new nsVisualContainer2d();

    _rect = CreateRect(600, 300);
    _userRect = CreateRect(500, 100);

    _circle = new nsVisualCircle();
    _circle->radius = 100;
    _stage->AddChild(_circle);

    _userCircle = new nsVisualCircle();
    _userCircle->radius = 200;
    _userCircle->origin.pos = {300, 300};
    _stage->AddChild(_userCircle);

    _rectColliding = true;

    return true;
}

void nsCollisionTest::Release() {
    _stage->Destroy();
}

void nsCollisionTest::Loop(float deltaTime) {
    auto size = nsAppUtils::GetClientSize();

    _circle->origin.pos = {
            size.x / 4,
            size.y / 2,
    };

    _rect->origin.pos = {
            size.x / 4 * 3,
            size.y / 2,
    };

    float angle = _rect->origin.angle;
    angle += deltaTime;
    _rect->origin.angle = angle;
}

void nsCollisionTest::Draw() {
    bool collided1 = false;
    bool collided2 = false;

    if (_rectColliding) {
        collided1 = nsCollisionMath::RectToRect(_rect->origin, _rect->rect, _userRect->origin, _userRect->rect);
        collided2 = nsCollisionMath::RectToCircle(_userRect->origin, _userRect->rect, _circle->origin, _circle->radius);
    } else {
        collided1 = nsCollisionMath::RectToCircle(_rect->origin, _rect->rect, _userCircle->origin, _userCircle->radius);
        collided2 = nsCollisionMath::CircleToCircle(_userCircle->origin, _userCircle->radius, _circle->origin, _circle->radius);
    }

    _rect->color = collided1 ? nsColor::red : nsColor::white;
    _circle->color = collided2 ? nsColor::red : nsColor::white;

    nsVisualSceneRender2d::DrawScene(_stage);
}

nsVisualRect *nsCollisionTest::CreateRect(float width, float height) {
    auto rect = new nsVisualRect();
    rect->rect = {-width / 3, -height / 3, width, height};
    _stage->AddChild(rect);
    return rect;
}

bool nsCollisionTest::OnPointerMove(float x, float y, int pointerId) {
    BaseFunctionalTest::OnPointerMove(x, y, pointerId);

    auto size = nsAppUtils::GetClientSize();

    nsVec2 pos = {x, size.y - y};

    _userRect->origin.pos = pos;
    _userCircle->origin.pos = pos;

    return true;
}
void nsCollisionTest::OnKeyDown(int key, bool rept) {
    BaseFunctionalTest::OnKeyDown(key, rept);
    if (key == NS_KEY_SPACE) {
        _rectColliding = !_rectColliding;
    }
}
void nsCollisionTest::OnMouseWheel(float delta) {
    BaseFunctionalTest::OnMouseWheel(delta);
    _angle += nsMath::ToRad(delta * 10.0f);
    _userRect->origin.angle = _angle;
}

