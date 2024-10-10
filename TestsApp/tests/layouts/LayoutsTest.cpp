//
// Created by Roman on 8/1/2024.
//

#include "LayoutsTest.h"
#include "Engine/display/VisualSceneRender2d.h"
#include "Engine/Platform.h"
#include "Engine/display/layouts/HGroupLayout.h"
#include "Engine/display/layouts/VGroupLayout.h"
#include "Engine/display/text/TextArea.h"
#include "Engine/display/button/TextButton.h"
#include "Engine/display/popup/PopupManager.h"
#include "Engine/display/Sprite.h"
#include "Engine/display/button/IconButton.h"
#include "Engine/display/popup/MessagePopup.h"
#include "nsLib/log.h"
#include "Engine/display/sprite/Sprite9Slice.h"

nsTextLabel *nsLayoutsTest::CreateLabel(const char *text, nsVisualContainer2d *container) {
    auto label = new nsTextLabel();
    label->drawFrame = true;
    label->text = text;
    label->origin.scale = nsVec2(0.5, 1);
    container->AddChild(label);
    return label;
}

void nsLayoutsTest::CreateGroupLayout() {

    auto group = new nsGroupLayout();
    group->drawFrame = true;
    group->xMin.Set(nsLayoutAnchor::NUMBER, 10);
    group->xMax.Set(nsLayoutAnchor::PERCENT, 0.5f);
    group->yMin.Set(nsLayoutAnchor::NUMBER, 10);
    group->yMax.Set(nsLayoutAnchor::PERCENT, 1.0 / 3);
    _root.AddChild(group);

    auto label = CreateLabel("Left, Bottom (0.1, 0.2)", group);
    label->xMin.Set(nsLayoutAnchor::PERCENT, 0.1);
    label->yMin.Set(nsLayoutAnchor::PERCENT, 0.2);

    label = CreateLabel("Right, Bottom (0.2 , 0.1)", group);
    label->xMax.Set(nsLayoutAnchor::PERCENT, 0.2);
    label->yMin.Set(nsLayoutAnchor::PERCENT, 0.1);

    label = CreateLabel("Left, Top (0.2 , 0.2)", group);
    label->xMin.Set(nsLayoutAnchor::PERCENT, 0.2);
    label->yMax.Set(nsLayoutAnchor::PERCENT, 0.2);

    label = CreateLabel("Right, Top (0.01 , 0.1)", group);
    label->xMax.Set(nsLayoutAnchor::PERCENT, 0.01);
    label->yMax.Set(nsLayoutAnchor::PERCENT, 0.1);

    label = CreateLabel("Center (0, -0.25)", group);
    label->xCenter.Set(nsLayoutAnchor::PERCENT, 0);
    label->yCenter.Set(nsLayoutAnchor::PERCENT, -0.25);

    label = CreateLabel("Stretched", group);
    label->yCenter.Set(nsLayoutAnchor::NUMBER, 0);
    label->xMax.Set(nsLayoutAnchor::NUMBER, 10);
    label->xMin.Set(nsLayoutAnchor::NUMBER, 10);
}

void nsLayoutsTest::CreateVGroupLayout() {
    auto group = new nsVGroupLayout();
    group->drawFrame = true;
    group->gap = 5;
    group->verticalAlign = nsAlign::CENTER;
    group->xMin.Set(nsLayoutAnchor::PERCENT, 0.5f);
    group->xMax.Set(nsLayoutAnchor::NUMBER, 10);
    group->yMin.Set(nsLayoutAnchor::NUMBER, 10);
    group->yMax.Set(nsLayoutAnchor::PERCENT, 1.0f/3);
    _root.AddChild(group);

    auto *back = new nsSprite();
    back->desc.color = {0, 0, 0, 0.5};
    back->xMin.SetNumber(20);
    back->xMax.SetNumber(20);
    back->yMin.SetNumber(20);
    back->yMax.SetNumber(20);
    group->AddChild(back);

    auto label = CreateLabel("No Padding", group);
    label->xMin.Set(nsLayoutAnchor::NUMBER, 10);

    label = CreateLabel("Gap 5", group);
    label->xMax.Set(nsLayoutAnchor::NUMBER, 10);

    for (int i = 0; i < 10; i++) {
        label = CreateLabel("Stretched", group);
        label->xMin.Set(nsLayoutAnchor::NUMBER, 10);
        label->xMax.Set(nsLayoutAnchor::NUMBER, 10);
    }

    auto button = new nsTextButton();
    button->xCenter.Set(nsLayoutAnchor::NUMBER, 0);
    button->label = "Test Popup";
    group->AddChild(button);
    button->SetClickHandler([]() -> void {
        nsMessagePopup::Show("Info", "This is\na message!\nLook at my awesome code!", {
                {"Yes", []() -> void { Log::Info("Yes! Clicked!"); }},
                {"No", []() -> void { Log::Info("No! Clicked!"); }},
        });
    });

    auto iconButton = new nsIconButton();
    iconButton->SetTexture("tests/particle.jpg");
    iconButton->xCenter.SetNumber(0);
    group->AddChild(iconButton);
}

void nsLayoutsTest::CreateHGroupLayout() {
    const auto group = new nsHGroupLayout();
    group->drawFrame = true;
    group->gap = 5;
    group->horizontalAlign = nsAlign::CENTER;
    group->xMin.Set(nsLayoutAnchor::NUMBER, 10);
    group->xMax.Set(nsLayoutAnchor::NUMBER, 10);
    group->yMin.Set(nsLayoutAnchor::PERCENT, 2.0 / 3);
    group->yMax.Set(nsLayoutAnchor::NUMBER, 10);
    _root.AddChild(group);

    auto *back = new nsSprite9Slice();
    back->desc.tex = _device->TextureLoad("default/ui/frame.png");
    back->desc.SetGrid(6, 6, 6, 6);
    back->desc.color = nsColor::red;
    back->xMin.SetNumber(20);
    back->xMax.SetNumber(20);
    back->yMin.SetNumber(20);
    back->yMax.SetNumber(20);
    group->AddChild(back);

    nsTextLabel *label;
    for (int i = 0; i < 3; i++) {
        label = CreateLabel("bottom", group);
        label->yMin.Set(nsLayoutAnchor::NUMBER, 10);
    }

    for (int i = 0; i < 3; i++) {
        label = CreateLabel("top", group);
        label->yMax.Set(nsLayoutAnchor::NUMBER, 10);
    }

    for (int i = 0; i < 3; i++) {
        label = CreateLabel("center", group);
        label->yCenter.Set(nsLayoutAnchor::NUMBER, 0);
    }

    for (int i = 0; i < 3; i++) {
        label = CreateLabel("size", group);
        label->yMin.Set(nsLayoutAnchor::NUMBER, 0);
        label->yMax.Set(nsLayoutAnchor::NUMBER, 0);
    }

    auto text = new nsTextArea();
    text->drawFrame = true;
    text->SetLineSpace(15);
    text->hAlign = nsAlign::CENTER;
    text->origin.scale = {0.5, 1};
    text->SetText("Hello!\nFucking TikTok\nFrom GROm!");
    text->yCenter.Set(nsLayoutAnchor::NUMBER, 0);
    group->AddChild(text);
}

bool nsLayoutsTest::Init() {
    //_root.origin.scale = nsVec2(2, 2);
    auto guiState = _device->StateLoad("default/rs/gui.txt");
    auto back = new nsSprite();
    back->renState = guiState;
    back->desc.size = { 100, 100 };
    back->desc.center = { 50, 50 };
    back->desc.color = nsColor(0, 1, 0, 0.2);
    back->xMax.SetNumber(20);
    back->yMax.SetNumber(20);
    back->xMin.SetNumber(20);
    back->yMin.SetNumber(20);
    _root.AddChild(back);

    CreateGroupLayout();
    CreateVGroupLayout();
    CreateHGroupLayout();

    nsPopupManager::Init();
    nsPopupManager::Shared()->overlayState = guiState;
    _root.AddChild(nsPopupManager::Shared()->GetRoot());
    return true;
}

void nsLayoutsTest::Release() {
    nsPopupManager::Release();
    _root.DestroyChildren();
}

void nsLayoutsTest::Loop(float deltaTime) {
    int w, h;
    App_GetPlatform()->GetClientSize(w, h);

    _root.SetWidth(float(w));
    _root.SetHeight(float(h));
}

void nsLayoutsTest::Draw() {
    nsVisualSceneRender2d::DrawScene(&_root);
}

IUserInput *nsLayoutsTest::GetInput() {
    return &_root;
}


