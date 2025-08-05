//
// Created by Roman on 9/18/2024.
//

#include "ScrollTest.h"
#include "Engine/Platform.h"
#include "Engine/display/VisualSceneRender2d.h"
#include "Engine/display/layouts/VGroupLayout.h"
#include "Engine/display/layouts/scroll/ScrollBar.h"
#include "Engine/display/sprite/Sprite.h"

bool nsScrollTest::Init() {
/*    auto content = new nsVGroupLayout();

    for (int i = 0; i < 100; i++) {
        auto label = new nsTextLabel();
        label->text.Format("label %i", i);
        content->AddChild(label);
    }

    content->yMax.SetNumber(0);

    _root.AddChild(content);*/

    auto barGroup = new nsVGroupLayout();
    barGroup->xMin.Set(nsLayoutAnchor::PERCENT, 2.0 / 3.0);
    barGroup->xMax.SetNumber(10);
    barGroup->yMax.SetNumber(10);
    barGroup->yMin.SetNumber(10);

    auto back = new nsSprite();
    back->desc.color.a = 0.5;
    back->xMin.SetNumber(0);
    back->xMax.SetNumber(0);
    back->yMin.SetNumber(0);
    back->yMax.SetNumber(0);
    barGroup->AddChild(back);

    auto bar = new nsScrollBar();
    bar->yMin.SetNumber(0);
    bar->yMax.SetNumber(0);
    bar->xMin.SetNumber(0);
    barGroup->AddChild(bar);
    _root.AddChild(barGroup);

    //_root.origin.scale = {2, 2};

    return true;
}

void nsScrollTest::Release() {
    _root.DestroyChildren();
}

void nsScrollTest::Loop(float deltaTime) {

}

void nsScrollTest::Draw() {
    int w,h;
    App_GetPlatform()->GetClientSize(w, h);

    _root.SetWidth((float)w);
    _root.SetHeight((float)h);

    nsVisualSceneRender2d::DrawScene(&_root);
}

IUserInput *nsScrollTest::GetInput() {
    return &_root;
}
