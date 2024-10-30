//
// Created by Roman on 10/30/2024.
//

#include "DebugDrawInfo.h"
#include "renderer/font/FontsCache.h"
#include "nsLib/color.h"

nsDebugDrawInfo::nsDebugDrawInfo(const nsVec2 &startPos) : _pos(startPos) {

}

void nsDebugDrawInfo::DrawLine(const char *text, const nsColor &c) {
    auto font = nsFontsCache::Shared()->SysFont();
    nsVec2  scale = {0.5, 1};
    font->Draw(text, _pos, scale, c);

    _pos.y += font->GetLineHeight() + 2;
}
