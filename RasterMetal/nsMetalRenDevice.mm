//
// Created by OpenCode
//

#include "nsMetalRenderDevice.h"

static nsMetalRenderDevice *g_shared = nullptr;

IRenDevice *GetRenDevice() {
    if (!g_shared) {
        g_shared = new nsMetalRenderDevice();
    }
    return g_shared;
}
