//
// Created by Roman on 10/14/2024.
//

#include "VisualContainerBuilder.h"
#include "Engine/display/VisualContainer2d.h"
#include "nsLib/log.h"

nsVisualObject2d *nsVisualContainerBuilder::Create(script_state_t *ss, nsVisualCreationContext2d *context) {
    return new nsVisualContainer2d();
}

bool nsVisualContainerBuilder::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, object, context)) {
        return false;
    }

    auto container = dynamic_cast<nsVisualContainer2d*>(object);
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
    } else {
        Log::Warning("Container children are not specified!");
    }

    return true;
}
