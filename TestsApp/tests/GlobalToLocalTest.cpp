//
// Created by Roman on 7/29/2024.
//

#include "GlobalToLocalTest.h"
#include "engine/RenAux.h"
#include "engine/Platform.h"
#include "engine/RenManager.h"
#include "engine/display/VisualSceneRender2d.h"

bool nsGlobalToLocalTest::Init() {
    _container.AddChild(&_rect);
    _rect.origin.pos = nsVec2(50, 100);
    _rect.origin.angle = nsMath::ToRad(45);
    return true;
}

void nsGlobalToLocalTest::Loop(float deltaTime) {
    float a = _container.origin.angle + deltaTime * nsMath::ToRad(45);
    _container.origin.angle = a;
}

void nsGlobalToLocalTest::Draw() {
    int w, h;
    App_GetPlatform()->GetClientSize(w, h);
    _container.origin.pos = nsVec2(float(w) / 2, float(h) / 2);

    nsVisualSceneRender2d::DrawScene(&_container);

    nsMatrix    m;
    m.Identity();
    g_renDev->LoadMatrix(m);

    nsRect  rect;
    _container.GetBounds(rect, nullptr);
    nsGizmos::DrawRect(rect, nsColor::gray);
}

void nsGlobalToLocalTest::Release() {

}

bool nsGlobalToLocalTest::OnPointerMove(float x, float y, int pointerId) {
    BaseFunctionalTest::OnPointerMove(x, y, pointerId);

    int w, h;
    App_GetPlatform()->GetClientSize(w, h);
    _rect.mousePos = nsVec2(x, float(h) - y);
    return true;
}

void nsGlobalToLocalRect::GetLocalBounds(nsRect &bounds) {
    bounds.x = 0;
    bounds.y = 0;
    bounds.width = _width;
    bounds.height = _height;
}

void nsGlobalToLocalRect::DrawContent(const nsVisualContext2d &context) {
    auto color = HitTest(mousePos.x, mousePos.y) ? nsColor::red : nsColor::white;
    RX_DrawRect(0, 0, _width, _height, color);
    nsGizmos::DrawCross(origin.ToLocal(mousePos), 20, nsColor::red);
}

void nsGlobalToLocalRect::Loop() {

}
