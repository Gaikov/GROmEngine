//
// Created by Roman on 1/27/2026.
//

#include "DefaultVisualObject.h"

#include "RenManager.h"
#include "renderer/font/FontsCache.h"

constexpr auto SIZE = 100.0f;
constexpr auto HALF_SIZE = SIZE / 2.0f;

nsDefaultVisualObject::nsDefaultVisualObject() {
    _sprite.center = {HALF_SIZE, HALF_SIZE};
    _sprite.size = {SIZE, SIZE};
    _renState = _device->StateLoad("default/rs/gui.ggrs");
}

void nsDefaultVisualObject::SetTagName(const char *tag) {
    _tagName = tag;
    const auto color = nsStr::GetBrightColor(tag);
    _sprite.color.FromRGBA(std::stoul(color, nullptr, 16));
}

const std::string & nsDefaultVisualObject::GetTagName() const {
    return _tagName;
}

const char * nsDefaultVisualObject::GetType() {
    return _tagName.c_str();
}

void nsDefaultVisualObject::GetLocalBounds(nsRect &bounds) {
    bounds.x = -HALF_SIZE;
    bounds.y = -HALF_SIZE;
    bounds.width = SIZE;
    bounds.height = SIZE;
}

void nsDefaultVisualObject::Loop() {
}

void nsDefaultVisualObject::DrawContent(const nsVisualContext2d &context) {
    _device->StateApply(_renState);
    _sprite.Draw(_device);

    const auto font = nsFontsCache::Shared()->SysFont();

    nsRect rect;
    font->GetBounds(_tagName.c_str(), rect);

    const nsVec2 pos = {- rect.width / 2, - rect.height / 2};
    font->Draw(_tagName.c_str(), pos, nsVec2(1, 1), nsColor::black);
}
