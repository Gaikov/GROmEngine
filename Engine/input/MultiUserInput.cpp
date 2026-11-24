//
// Created by Roman on 6/1/2024.
//

#include "MultiUserInput.h"

void nsMultiUserInput::OnKeyUp(int key, int mods) {
    for (auto ui : GetInteractiveItems()) {
        ui->OnKeyUp(key, mods);
    }
}

void nsMultiUserInput::OnKeyDown(int key, bool rept, int mods) {
    for (auto ui : GetInteractiveItems()) {
        ui->OnKeyDown(key, rept, mods);
    }
}

void nsMultiUserInput::OnChar(char ch) {
    for (auto ui : GetInteractiveItems()) {
        ui->OnChar(ch);
    }
}

bool nsMultiUserInput::OnPointerUp(const nsPointerData &pointer) {
    return DispatchPointer(pointer, &IPointersInput::OnPointerUp);
}

bool nsMultiUserInput::OnPointerDown(const nsPointerData &pointer) {
    return DispatchPointer(pointer, &IPointersInput::OnPointerDown);
}

bool nsMultiUserInput::OnPointerMove(const nsPointerData &pointer) {
    return DispatchPointer(pointer, &IPointersInput::OnPointerMove);
}

bool nsMultiUserInput::DispatchPointer(const nsPointerData &pointer,
                                     bool (IPointersInput::*handler)(const nsPointerData &)) {
    const auto dispatch = [this, handler](const nsPointerData &target) {
        for (auto ui : GetInteractiveItems()) {
            if ((ui->*handler)(target)) return true;
        }
        return false;
    };
    if (!_pointerTransform) return dispatch(pointer);

    auto target = pointer;
    target.pos = _pointerTransform->ScreenToTarget(pointer.pos.x, pointer.pos.y);
    return dispatch(target);
}

bool nsMultiUserInput::OnMouseWheel(float delta) {
    for (auto ui : GetInteractiveItems()) {
        if (ui->OnMouseWheel(delta)) {
            return true;
        }
    }
    return false;
}

void nsMultiUserInput::ClearInputs() {
    _interactiveItems.clear();
}

void nsMultiUserInput::AddInput(IUserInput *input) {
    _interactiveItems.push_back(input);
}

void nsMultiUserInput::OnPointerCancel(const nsPointerData &pointer) {
    for (auto ui : GetInteractiveItems()) {
        ui->OnPointerCancel(pointer);
    }
}
