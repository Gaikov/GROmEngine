//
// Created by Roman on 9/6/2024.
//

#include "SoftInputKeyboard.h"
#include "input/SoftKeyboard.h"

void nsSoftInputKeyboard::Show() {
    nsSoftKeyboard::Shared()->Activate(true);
}
void nsSoftInputKeyboard::Hide() {
    nsSoftKeyboard::Shared()->Activate(false);
}
bool nsSoftInputKeyboard::IsActive() {
    return nsSoftKeyboard::Shared()->IsActive();
}
