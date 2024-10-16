//
// Created by Roman on 10/14/2024.
//

#include "LayoutRefBuilder.h"
#include "Core/ParserUtils.h"
#include "Core/ParseFile.h"
#include "nsLib/log.h"

nsVisualObject2d *nsLayoutRefBuilder::Create(script_state_t *ss, nsVisualCreationContext2d *context) {
    auto refPath = ParseString(ss, "source");
    if (refPath.IsEmpty()) {
        Log::Warning("Invalid layout reference: %s", refPath.AsChar());
        return nullptr;
    }

    return context->Create(refPath);
}

bool nsLayoutRefBuilder::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    auto refPath = ParseString(ss, "source");
    nsParseFile pf;
    auto source = pf.BeginFile(refPath);
    if (ps_block_begin(source, nullptr)) {
        auto builder = context->GetBuilder(ps_block_name(source));
        if (builder) {
            return builder->Parse(ss, object, context);
        }
    }
    return false;
}

nsVisualObject2d *nsLayoutRefBuilder::CreateDefault() {
    return nullptr;
}
