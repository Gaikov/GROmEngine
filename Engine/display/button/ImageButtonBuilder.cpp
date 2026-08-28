//
// Created by Roman on 2/3/2025.
//

#include "ImageButtonBuilder.h"
#include "ImageButton.h"
#include "Core/ParserUtils.h"
#include "RenManager.h"
#include "Engine/assets/VisualAssetsContext.h"
#include "renderer/font/FontsCache.h"

namespace {
bool HasLabel(const nsImageButton *button) {
    const auto &color = button->textColor;
    return button->font || !button->text.IsEmpty() || button->labelPos != nsVec2()
           || color.r != 1 || color.g != 1 || color.b != 1 || color.a != 1;
}
}

nsVisualObject2d *nsImageButtonBuilder::CreateDefault(nsVisualCreationContext2d *) {
    return new nsImageButton();
}

bool nsImageButtonBuilder::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    auto dev = nsRenDevice::Shared()->Device();

    auto button = Cast<nsImageButton>(o);
    button->renState = dev->StateLoad(context->assetsContext->ParseAssetPath(ss, "renState"));

    button->up      .Parse(ss, context->assetsContext.get(), "up");
    button->over    .Parse(ss, context->assetsContext.get(), "over");
    button->down    .Parse(ss, context->assetsContext.get(), "down");
    button->disabled.Parse(ss, context->assetsContext.get(), "disabled");

    if (ps_block_begin(ss, "label")) {
        if (const auto font = nsFontsCache::Shared()->LoadFont(context->assetsContext->ParseAssetPath(ss, "font"))) {
            button->font = font;
        }

        if (ps_var_begin(ss, "text")) {
            button->text = ps_var_str(ss);
        }

        if (ps_var_begin(ss, "color")) {
            ps_var_4f(ss, button->textColor);
        }

        button->AlignText(nsAlign::CENTER, nsAlign::CENTER);

        nsVec2 offset;
        if (ps_var_begin(ss, "offset")) {
            ps_var_2f(ss, offset);
            button->labelPos += offset;
        }

        ps_block_end(ss);
    }

    return nsVisualBuilder2d::Parse(ss, o, context);
}

bool nsImageButtonBuilder::SerializeProps(nsScriptSaver &saver, nsVisualObject2d *o,
                                          nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::SerializeProps(saver, o, context)) {
        return false;
    }

    const auto button = Cast<nsImageButton>(o);
    if (!button) {
        return false;
    }

    if (button->renState) {
        const auto dev = nsRenDevice::Shared()->Device();
        context->assetsContext->SaveAssetPath(saver, "renState", dev->StateGetPath(button->renState));
    }

    button->up.Save(saver, context->assetsContext.get(), "up");
    button->over.Save(saver, context->assetsContext.get(), "over");
    button->down.Save(saver, context->assetsContext.get(), "down");
    button->disabled.Save(saver, context->assetsContext.get(), "disabled");

    if (HasLabel(button) && saver.BlockBegin("label")) {
        if (button->font) {
            context->assetsContext->SaveAssetPath(saver, "font", button->font->GetPath());
        }

        saver.VarString("text", button->text);
        saver.VarFloat4("color", button->textColor, nsColor::white);
        const nsVec2 offset = button->labelPos
                              - button->ComputeAlignedTextPosition(nsAlign::CENTER, nsAlign::CENTER);
        saver.VarFloat2("offset", offset, nsVec2());
        saver.BlockEnd();
    }

    return true;
}
