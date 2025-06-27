//
// Created by Roman on 11/5/2024.
//

#include "VisualsPoolDebugDraw.h"

#include "utils/DebugDrawInfo.h"
#include "nsLib/StrTools.h"
#include "renderer/particles/ParticlesPool.h"
#include "Core/Config.h"
#include "display/pool/LayoutsPool.h"

static nsVar *vis_pool_allocations = nullptr;

nsVisualsPoolDebugDraw::nsVisualsPoolDebugDraw() {
    vis_pool_allocations = g_cfg->RegVar("vis_pool_allocations", "0");
}

void nsVisualsPoolDebugDraw::Draw() {
    if (vis_pool_allocations->Bool()) {
        nsDebugDrawInfo info({5, 50});
        nsString text;

        text.Format("Unused visuals: %i", nsLayoutsPool::Shared()->GetAvailableAmount());
        info.DrawLine(text);

        text.Format("Total allocated visuals: %i", nsLayoutsPool::Shared()->GetAllocatedAmount());
        info.DrawLine(text);

        text.Format("Allocated particles: %i", nsParticlesPool::Shared()->GetCapacity());
        info.DrawLine(text);
    }
}
