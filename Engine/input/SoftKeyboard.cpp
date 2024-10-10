//
// Created by Roman on 8/21/2024.
//

#include "SoftKeyboard.h"
#include "Engine/KeyCodes.h"
#include "nsLib/log.h"
#include "display/layouts/HGroupLayout.h"
#include "display/Sprite.h"

#define l(ch) new nsLetterKeyButton(ch, this)
#define k(keyCode, iconFile) new nsKeyCodeButton(keyCode, iconFile, this)

class nsKeyboardLayout : public nsVGroupLayout {
protected:
    bool OnPointerUp(float x, float y, int pointerId) override {
        nsVGroupLayout::OnPointerUp(x, y, pointerId);

        auto pos = origin.ToLocal({x, y});
        nsRect bounds = {0, 0, GetContentWidth(), GetContentHeight()};
        if (!bounds.IsInside(pos)) {
            visible = false;
        }
        return true;
    }
    bool OnPointerDown(float x, float y, int pointerId) override {
        nsVGroupLayout::OnPointerDown(x, y, pointerId);
        return true;
    }
    bool OnPointerMove(float x, float y, int pointerId) override {
        nsVGroupLayout::OnPointerMove(x, y, pointerId);
        return true;
    }
};

nsVisualObject2d *nsSoftKeyboard::GetRoot() {
    return _root;
}

void nsSoftKeyboard::SetScale(float s) {
    _root->origin.scale = {s, s};
}

bool nsSoftKeyboard::OnInit() {
    if (!nsSubSystem::OnInit()) {
        return false;
    }
    _root = new nsKeyboardLayout();

    auto *back = new nsSprite();
    back->desc.color = {0, 0, 0, 0.95};
    back->xMin.SetNumber(0);
    back->xMax.SetNumber(0);
    back->yMin.SetNumber(0);
    back->yMax.SetNumber(0);
    _root->AddChild(back);

    auto spaceKey = k(NS_KEY_SPACE, nullptr);
    spaceKey->iconColor = {1, 1, 1, 0};
    auto &spaceIcon = spaceKey->icon;
    spaceIcon.size = {180, 16};
    spaceIcon.tex = nullptr;

    std::vector<std::vector<nsBaseButton *> > buttons = {
        {
            l('1'), l('2'), l('3'), l('4'), l('5'), l('6'), l('7'), l('8'), l('9'), l('0'), l('-'),
            k(NS_KEY_BACKSPACE, "default/keys/back-space.png")
        },
        {l('q'), l('w'), l('e'), l('r'), l('t'), l('y'), l('u'), l('i'), l('o'), l('p'), l('['), l(']')},
        {
            l('a'), l('s'), l('d'), l('f'), l('g'), l('h'), l('j'), l('k'), l('l'), l(';'), l('\''),
            k(NS_KEY_ENTER, "default/keys/enter.png")
        },
        {l('z'), l('x'), l('c'), l('v'), l('b'), l('n'), l('m'), l(','), l('.'), l('!'), l('?'), l('@')},
        {k(NS_KEY_LSHIFT, "default/keys/shift.png"), spaceKey, k(NS_KEY_LSHIFT, "default/keys/shift.png")},
    };

    for (auto &row: buttons) {
        auto rowGroup = new nsHGroupLayout();
        rowGroup->boundsType = nsBaseLayout::CHILDREN;
        rowGroup->gap = 5;
        rowGroup->xCenter.SetNumber(0);
        _root->AddChild(rowGroup);

        for (const auto button: row) {
            rowGroup->AddChild(button);
            if (const auto letter = dynamic_cast<nsLetterKeyButton *>(button)) {
                _letters.push_back(letter);
            }
        }
    }

    _root->SetContentHeight(MAX_HEIGHT);
    _root->gap = 5;
    _root->verticalAlign = nsAlign::CENTER;
    _root->xMin.SetNumber(0);
    _root->xMax.SetNumber(0);
    _root->yMin.SetNumber(0);

    Activate(false);
    return true;
}

void nsSoftKeyboard::OnRelease() {
    if (const auto parent = _root->GetParent()) {
        parent->RemoveChild(_root);
    }
    _root->Destroy();
    nsSubSystem::OnRelease();
}

void nsSoftKeyboard::OnChar(char ch) {
    if (_handler) {
        _handler->OnChar(ch);
    }
}

void nsSoftKeyboard::OnKeyDown(int keyCode) {
    if (keyCode == NS_KEY_LSHIFT) {
        _caps = !_caps;
        for (auto letter: _letters) {
            letter->SetCaps(_caps);
        }
    }

    if (_handler) {
        _handler->OnKeyDown(keyCode, false);
        _handler->OnKeyUp(keyCode);
    }
}

void nsSoftKeyboard::UpdateScale(float screenWidth, float screenHeight) {
    float maxHeight = screenHeight / 2;

    float sw = screenWidth / MAX_WIDTH;
    float sh = maxHeight / MAX_HEIGHT;

    SetScale(std::min(sw, sh));
}

bool nsSoftKeyboard::IsActive() {
    return _root->visible;
}

void nsSoftKeyboard::Activate(bool active) {
    _root->visible = active;
}

void nsSoftKeyboard::SetHandler(IKeyboardInput *handler) {
    _handler = handler;
}
