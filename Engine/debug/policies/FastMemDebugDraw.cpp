//
// Created by Roman on 6/26/2025.
//

#include "FastMemDebugDraw.h"

#include "Core/Config.h"
#include "nsLib/StrTools.h"
#include "Core/memory/FastMemManager.h"
#include "utils/AppUtils.h"
#include "utils/DebugDrawInfo.h"

static nsVar *mem_pool_info = nullptr;

nsFastMemDebugDraw::nsFastMemDebugDraw() {
    mem_pool_info = g_cfg->RegVar("mem_pool_info", "0", 0);
}

void nsFastMemDebugDraw::Draw() {
    if (!mem_pool_info->Bool()) {
        return;
    }

    const auto size = nsAppUtils::GetClientSize();
    nsDebugDrawInfo drawInfo({10, size.y / 2});

    auto fm = nsFastMemManager::Shared();
    for (int i = 0; i < fm->GetPoolsAmount(); i++) {
        nsFastMemManager::PoolInfo info = {};
        fm->GetPoolInfo(i, info);

        nsString str;
        str.Format("%s: %i/%i", info.debugName, info.usedBlocks, info.totalBlocks);
        drawInfo.DrawLine(str);
    }
}
