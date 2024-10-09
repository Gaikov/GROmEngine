//
// Created by Roman on 8/28/2024.
//

#pragma once
#include "engine/display/button/IconButton.h"
#include "engine/display/button/TextButton.h"

class ISoftKeyHandler {
public:
    virtual ~ISoftKeyHandler() = default;
    virtual void OnChar(char ch) = 0;
    virtual void OnKeyDown(int keyCode) = 0;
};

class nsLetterKeyButton : public nsTextButton {
public:
    nsLetterKeyButton(char letter, ISoftKeyHandler *handler);
    void SetCaps(bool upper);

private:
    char _letter;
    ISoftKeyHandler *_handler;
};

class nsKeyCodeButton : public nsIconButton {
public:
    nsKeyCodeButton(int keyCode, const char *iconFile, ISoftKeyHandler *handler);

private:
    int _keyCode;
    ISoftKeyHandler *_handler;
};