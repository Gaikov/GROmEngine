//
// Created by Roman on 4/16/2025.
//

#include "Env.h"

#ifdef WEB_ASM
#include "env/web/WebEnv.h"
#else
#include "env/desktop/DesktopEnv.h"
#endif

nsEnv* nsEnv::_shared = nullptr;

bool nsEnv::Create() {
#ifdef WEB_ASM
    _shared = new nsWebEnv();
#else
    _shared = new nsDesktopEnv();
#endif
    return _shared->Init();
}

void nsEnv::Destroy() {
    delete _shared;
    _shared = nullptr;
}
