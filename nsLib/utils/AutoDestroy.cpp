//
// Created by Roman on 5/20/2025.
//

#include "AutoDestroy.h"
nsAutoDestroy::~nsAutoDestroy() {
    for (auto &func : _destroyFuncs) {
        func();
    }
}

void nsAutoDestroy::AddDestroyCallback(const destroyFunc_t &func) {
    _destroyFuncs.push_back(func);
}
