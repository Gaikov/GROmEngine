//
// Created by Roman on 10/30/2024.
//

#include "DebugDrawInfo.h"
#include "renderer/font/FontsCache.h"
#include "nsLib/color.h"

nsDebugDrawInfo::nsDebugDrawInfo(const nsVec2 &startPos) : _pos(startPos) {

}

void nsDebugDrawInfo::DrawLine(const char *text, const nsColor &c) {
    DrawLine(_pos, text, c);

    const auto font = nsFontsCache::Shared()->SysFont();
    _pos.y += font->GetLineHeight() + 2;
}

void nsDebugDrawInfo::DrawLine(const nsVec2 &pos, const char *text, const nsColor &c) {
    const auto font = nsFontsCache::Shared()->SysFont();
    const nsVec2  scale = {0.5, 1};
    font->Draw(text, pos, scale, c);
}
