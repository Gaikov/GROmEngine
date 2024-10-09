//
// Created by Roman on 7/25/2024.
//
#include "BaseFunctionalTest.h"
#include "Platform.h"

BaseFunctionalTest::BaseFunctionalTest() :
        _device(nsRenDevice::Shared()->Device()) {
}

bool BaseFunctionalTest::OnPointerUp(float x, float y, int pointerId) {
    auto input = GetInput();
    if (input) {
        nsVec2 pos = WindowToScreen({x, y});
        return input->OnPointerUp(pos.x, pos.y, pointerId);
    }
    return false;
}

bool BaseFunctionalTest::OnPointerDown(float x, float y, int pointerId) {
    auto input = GetInput();
    if (input) {
        nsVec2 pos = WindowToScreen({x, y});
        return input->OnPointerDown(pos.x, pos.y, pointerId);
    }
    return false;
}

bool BaseFunctionalTest::OnPointerMove(float x, float y, int pointerId) {
    auto input = GetInput();
    if (input) {
        auto pos = WindowToScreen({x, y});
        return input->OnPointerMove(pos.x, pos.y, pointerId);
    }
    return false;
}

void BaseFunctionalTest::OnKeyUp(int key) {
    auto input = GetInput();
    if (input) {
        input->OnKeyUp(key);
    }
}

void BaseFunctionalTest::OnKeyDown(int key, bool rept) {
    auto input = GetInput();
    if (input) {
        input->OnKeyDown(key, rept);
    }
}

void BaseFunctionalTest::OnChar(char ch) {
    auto input = GetInput();
    if (input) {
        input->OnChar(ch);
    }
}

void BaseFunctionalTest::OnMouseWheel(float delta) {
    auto input = GetInput();
    if (input) {
        input->OnMouseWheel(delta);
    }
}

IUserInput *BaseFunctionalTest::GetInput() {
    return nullptr;
}

nsVec2 BaseFunctionalTest::WindowToScreen(const nsVec2 &winPos) {
    int w, h;
    App_GetPlatform()->GetClientSize(w, h);
    return {winPos.x, float(h) - winPos.y};
}

void BaseFunctionalTest::OnPointerCancel(int pointerId) {

}
