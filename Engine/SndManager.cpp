// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SndManager.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "SndManager.h"

bool nsSoundDevice::OnInit() {
    nsSubSystem::OnInit();

    _device = GetSoundDevice();
    return _device && _device->Init();
}

void nsSoundDevice::OnRelease() {

    if (_device) {
        _device->Release();
        _device = nullptr;
    }

    nsSubSystem::OnRelease();
}
