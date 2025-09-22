//
// Created by Roman on 3/26/2024.
//

#ifndef _GROM_PLATFORM_DESKTOPKEYNAMES_H
#define _GROM_PLATFORM_DESKTOPKEYNAMES_H

#include "DesktopCommon.h"
#include "nsLib/StrTools.h"

class DesktopKeyNames {
public:
    DesktopKeyNames();

    const char* GetName(int key);

private:
    void SetName(const char *name, int key);

    nsString _names[GLFW_KEY_LAST + 1];
};


#endif //_GROM_PLATFORM_DESKTOPKEYNAMES_H
