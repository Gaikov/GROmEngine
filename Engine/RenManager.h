// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RenManager.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "RenDevice.h"
#include "nsLib/SubSystem.h"

struct nsFont;

class nsRenDevice : public nsSubSystem<nsRenDevice>
{
public:
    static void Invalidate_f(int argc, const char* argv[]);
    static void Portrait_f(int argc, const char* argv[]);

	static bool		RegVars();

    inline IRenDevice*	Device() {
        return _current;
    }

private:
    bool OnInit() override;
    void OnRelease() override;

private:
    IRenDevice  *_current;
};

/**
 * @deprecated
 */
extern IRenDevice	*g_renDev;