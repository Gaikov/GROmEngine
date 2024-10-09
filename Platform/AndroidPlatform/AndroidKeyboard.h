//
// Created by Roman on 4/11/2024.
//

#ifndef _GROM_ANDROIDKEYBOARD_H
#define _GROM_ANDROIDKEYBOARD_H


#include "nsLib/StrTools.h"
#include "AndroidKeyCodes.h"

class AndroidKeyboard {
public:
    AndroidKeyboard();

    const char *GetName(int key);

    void SetKeyPressed(int key, bool pressed);

    bool IsKeyPressed(int key) { return _pressed[key]; }

private:
    void SetName(const char *name, int key);

    nsString _names[KEYBOARD_MAX_KEYS];
    bool _pressed[KEYBOARD_MAX_KEYS] = {};
};


#endif //_GROM_ANDROIDKEYBOARD_H
