//
// Created by Roman on 10/14/2024.
//

#include "TextLabelBuilder.h"
#include "TextLabel.h"
#include "renderer/font/FontsCache.h"
#include "Core/ParserUtils.h"
#include "RenManager.h"

nsVisualObject2d *nsTextLabelBuilder::CreateDefault() {
    return new nsTextLabel();
}

bool nsTextLabelBuilder::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, object, context)) {
        return false;
    }

    const auto label = dynamic_cast<nsTextLabel*>(object);
    if (const auto font = nsFontsCache::Shared()->LoadFont(context->ParseAssetPath(ss, "font"))) {
        label->font = font;
    }
    if (const auto state = nsRenDevice::Shared()->Device()->StateLoad(context->ParseAssetPath(ss, "renState"))) {
        label->renState = state;
    }

    label->text = ParseStrP(ss, "text", label->text);
    if (ps_var_begin(ss, "color")) {
        ps_var_4f(ss, label->color);
    }

    label->drawFrame = ParseBool(ss, "drawFrame", label->drawFrame);
    label->hAlign = nsAlign::Parse(ss, "hAlign", label->hAlign);
    label->letterSpace = ParseFloat(ss, "letterSpace", label->letterSpace);

    return true;
}

bool nsTextLabelBuilder::SerializeProps(nsScriptSaver &saver, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::SerializeProps(saver, o, context)) {
        return false;
    }

    const auto label = Cast<nsTextLabel>(o);
    if (label->font) {
        context->SaveAssetPath(saver, "font", label->font->GetPath());
    }

    if (label->renState) {
        const auto path = nsRenDevice::Shared()->Device()->StateGetPath(label->renState);
        context->SaveAssetPath(saver, "renState", path);
    }

    saver.VarString("text", label->text);
    saver.VarFloat4("color", label->color, nsColor::white);
    saver.VarBool("drawFrame", label->drawFrame, false);
    nsAlign::Save(saver, "hAlign", label->hAlign, nsAlign::BEGIN);
    saver.VarFloat("letterSpace", label->letterSpace, 0.0f);

    return true;
}