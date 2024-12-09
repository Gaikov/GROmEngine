// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SndManager.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "SoundBase/SndDevice.h"
#include "nsLib/SubSystem.h"

class nsSoundDevice : public nsSubSystem<nsSoundDevice>
{
public:
    inline ISndDevice* Device() {
        return _device;
    }

protected:
    bool OnInit() override;
    void OnRelease() override;


private:
    ISndDevice  *_device = nullptr;
};

