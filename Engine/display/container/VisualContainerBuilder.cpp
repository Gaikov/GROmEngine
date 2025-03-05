//
// Created by Roman on 10/14/2024.
//

#include "VisualContainerBuilder.h"
#include "Engine/display/container/VisualContainer2d.h"
#include "nsLib/log.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsVisualContainerBuilder::CreateDefault() {
    return new nsVisualContainer2d();
}

bool nsVisualContainerBuilder::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, object, context)) {
        return false;
    }

    auto container = Cast<nsVisualContainer2d>(object);
    if (!container) {
        return false;
    }

    if (ps_block_begin(ss, "children")) {
        if (ps_block_begin(ss, nullptr)) {
            do {
                auto child = context->Create(ss);
                if (child) {
                    container->AddChild(child);
                }
            } while (ps_block_next(ss));
            ps_block_end(ss);
        }
        ps_block_end(ss);
    }

    if (ps_block_begin(ss, "masking")) {
        do {
            auto maskId = ParseString(ss, "mask");
            auto mask = dynamic_cast<nsVisualMask*>(container->GetChildById(maskId));
            if (mask) {
                auto maskedId = ParseString(ss, "masked");
                auto masked = container->GetChildById(maskedId);
                if (masked) {
                    masked->AddMask(mask);
                } else {
                    Log::Warning("Invalid masked object: %s", maskedId.AsChar());
                }
            } else {
                Log::Warning("invalid mask object: %s", maskId.AsChar());
            }
        } while (ps_block_next(ss));
        ps_block_end(ss);
    }

    if (ps_var_begin(ss, "interactiveChildren")) {
        container->interactiveChildren = ps_var_f(ss) != 0.0f;
    }

    return true;
}
