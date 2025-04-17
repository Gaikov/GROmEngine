//
// Created by Roman on 4/16/2025.
//

#include "Env.h"

nsEnv* nsEnv::_shared = nullptr;

bool nsEnv::Create() {
    if (!_shared) {
        _shared = new nsEnv();
        return _shared->Init();
    }
    return true;
}

void nsEnv::Destroy() {
    delete _shared;
    _shared = nullptr;
}
