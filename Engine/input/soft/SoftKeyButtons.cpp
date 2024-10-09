//
// Created by Roman on 8/28/2024.
//

#include "SoftKeyButtons.h"

nsLetterKeyButton::nsLetterKeyButton(const char letter, ISoftKeyHandler *handler)
    : _letter(letter),
      _handler(handler) {

    label.Format("%c", _letter);
    SetClickHandler([this]()-> void {
        _handler->OnChar(_letter);
    });

    labelScale = {1, 1};
}

void nsLetterKeyButton::SetCaps(const bool upper) {
    if (upper) {
        _letter = static_cast<char>(toupper(_letter));
    } else {
        _letter = static_cast<char>(tolower(_letter));
    }
    label.Format("%c", _letter);
}

nsKeyCodeButton::nsKeyCodeButton(int keyCode, const char *iconFile, ISoftKeyHandler *handler)
    : _keyCode(keyCode),
      _handler(handler) {

    SetTexture(iconFile);
    SetClickHandler([this]()->void {
        _handler->OnKeyDown(_keyCode);
    });
}
