//
// Created by Roman on 7/25/2024.
//
#include "BaseFunctionalTest.h"
#include "Platform.h"

BaseFunctionalTest::BaseFunctionalTest() :
        _device(nsRenDevice::Shared()->Device()) {
}

bool BaseFunctionalTest::OnPointerUp(const nsPointerData &pointer) {
    auto input = GetInput();
    if (input) {
        auto target = pointer;
        target.pos = WindowToScreen(pointer.pos);
        return input->OnPointerUp(target);
    }
    return false;
}

bool BaseFunctionalTest::OnPointerDown(const nsPointerData &pointer) {
    auto input = GetInput();
    if (input) {
        auto target = pointer;
        target.pos = WindowToScreen(pointer.pos);
        return input->OnPointerDown(target);
    }
    return false;
}

bool BaseFunctionalTest::OnPointerMove(const nsPointerData &pointer) {
    auto input = GetInput();
    if (input) {
        auto target = pointer;
        target.pos = WindowToScreen(pointer.pos);
        return input->OnPointerMove(target);
    }
    return false;
}

void BaseFunctionalTest::OnKeyUp(int key, int mods) {
    auto input = GetInput();
    if (input) {
        input->OnKeyUp(key, mods);
    }
}

void BaseFunctionalTest::OnKeyDown(int key, bool rept, int mods) {
    auto input = GetInput();
    if (input) {
        input->OnKeyDown(key, rept, mods);
    }
}

void BaseFunctionalTest::OnChar(char ch) {
    auto input = GetInput();
    if (input) {
        input->OnChar(ch);
    }
}

bool BaseFunctionalTest::OnMouseWheel(float delta) {
    auto input = GetInput();
    if (input) {
        return input->OnMouseWheel(delta);
    }
    return false;
}

IUserInput *BaseFunctionalTest::GetInput() {
    return nullptr;
}

nsVec2 BaseFunctionalTest::WindowToScreen(const nsVec2 &winPos) {
    int w, h;
    App_GetPlatform()->GetClientSize(w, h);
    return {winPos.x, float(h) - winPos.y};
}

void BaseFunctionalTest::OnPointerCancel(const nsPointerData &) {

}
