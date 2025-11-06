//
// Created by Roman on 11/6/2025.
//

#include "DefaultVisualBuilder2d.h"
#include "RenManager.h"
#include "Core/ParserUtils.h"
#include "display/container/VisualContainer2d.h"
#include "display/sprite/Sprite.h"
#include "display/text/TextArea.h"


nsVisualObject2d * nsDefaultVisualBuilder2d::CreateDefault() {
    return new nsVisualContainer2d();
}

bool nsDefaultVisualBuilder2d::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, o, context)) {
        return false;
    }

    const auto container = Cast<nsVisualContainer2d>(o);

    const auto dev = nsRenDevice::Shared()->Device();
    const auto rs = dev->StateLoad("default/rs/gui.txt");

    const auto s = new nsSprite();
    s->desc.size = { 100, 100 };
    s->renState = rs;

    std::string name = ps_block_name(ss);
    name += container->id.c_str();
    const auto color = nsStr::GetBrightColor(name.c_str());
    s->desc.color.FromRGBA(std::stoul(color, nullptr, 16));

    container->AddChild(s);

    auto text = new nsTextArea();
    nsString label;
    label.Format("%s\n(%s)", ps_block_name(ss), container->id.c_str());

    text->hAlign = nsAlign::CENTER;
    text->SetText(label);
    text->renState = rs;
    container->AddChild(text);

    nsRect bounds;
    text->GetLocalBounds(bounds);
    const auto size = bounds.GetSize();
    text->origin.pos = -bounds.GetPos() - size / 2;

    const auto maxSize = std::max(size.x, size.y) + 10;
    const auto frameSize = nsVec2(maxSize, maxSize);
    s->desc.center = frameSize / 2;
    s->desc.size = frameSize;

    return true;
}
