//
// Created by Roman on 5/24/2024.
//

#include "SoftInputEmpty.h"
#include "nsLib/log.h"

void SoftInputEmpty::Show() {
    Log::Info("SoftInput -> Show");
}

void SoftInputEmpty::Hide() {
    Log::Info("SoftInput -> Hide");
}

bool SoftInputEmpty::IsActive() {
    return false;
}
