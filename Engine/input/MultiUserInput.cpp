//
// Created by Roman on 6/1/2024.
//

#include "MultiUserInput.h"

void nsMultiUserInput::OnKeyUp(int key) {
    for (auto ui : GetInteractiveItems()) {
        ui->OnKeyUp(key);
    }
}

void nsMultiUserInput::OnKeyDown(int key, bool rept) {
    for (auto ui : GetInteractiveItems()) {
        ui->OnKeyDown(key, rept);
    }
}

void nsMultiUserInput::OnChar(char ch) {
    for (auto ui : GetInteractiveItems()) {
        ui->OnChar(ch);
    }
}

bool nsMultiUserInput::OnPointerUp(float x, float y, int pointerId) {
    auto pos = ScreenToTarget(x, y);
    for (auto ui : GetInteractiveItems()) {
        if (ui->OnPointerUp(pos.x, pos.y, pointerId)) {
            return true;
        }
    }
    return false;
}

bool nsMultiUserInput::OnPointerDown(float x, float y, int pointerId) {
    auto pos = ScreenToTarget(x, y);
    for (auto ui : GetInteractiveItems()) {
        if (ui->OnPointerDown(pos.x, pos.y, pointerId)) {
            return true;
        }
    }
    return false;
}

bool nsMultiUserInput::OnPointerMove(float x, float y, int pointerId) {
    auto pos = ScreenToTarget(x, y);
    for (auto ui : GetInteractiveItems()) {
        if (ui->OnPointerMove(pos.x, pos.y, pointerId)) {
            return true;
        }
    }
    return false;
}

bool nsMultiUserInput::OnMouseWheel(float delta) {
    for (auto ui : GetInteractiveItems()) {
        if (ui->OnMouseWheel(delta)) {
            return true;
        }
    }
    return false;
}

nsVec2 nsMultiUserInput::ScreenToTarget(float x, float y) const {
    if (_pointerTransform) {
        return _pointerTransform->ScreenToTarget(x, y);
    }
    return {x, y};
}

void nsMultiUserInput::ClearInputs() {
    _interactiveItems.clear();
}

void nsMultiUserInput::AddInput(IUserInput *input) {
    _interactiveItems.push_back(input);
}

void nsMultiUserInput::OnPointerCancel(int pointerId) {

}
