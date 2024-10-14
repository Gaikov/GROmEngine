//
// Created by Roman on 10/14/2024.
//

#include "VisualFactory2d.h"
#include "nsLib/log.h"
#include "Core/ParseFile.h"
#include "display/sprite/SpriteBuilder.h"
#include "display/text/TextLabelBuilder.h"
#include "display/container/VisualContainerBuilder.h"

nsVisualFactory2d::nsVisualFactory2d() {
    RegisterBuilderWithName<nsSpriteBuilder>();
    RegisterBuilderWithName<nsTextLabelBuilder>();
    RegisterBuilderWithName<nsVisualContainerBuilder>();
}

void nsVisualFactory2d::RegisterBuilder(const char *name, nsVisualBuilder2d::sp_t &builder) {
    auto it = _builders.find(name);
    if (it != _builders.end()) {
        Log::Warning("overriding layout builder: %s", name);
    }
    _builders[name] = builder;
}

nsVisualObject2d *nsVisualFactory2d::Create(script_state_t *ss) {
    auto builder = GetBuilder(ps_block_name(ss));
    if (builder) {
        auto object = builder->Create(ss, this);
        if (object) {
            if (builder->Parse(ss, object, this)) {
                return object;
            } else {
                object->Destroy();
            }
        }
    }
    return nullptr;
}

nsVisualObject2d *nsVisualFactory2d::Create(const char *filePath) {
    nsParseFile pf;
    auto ss = pf.BeginFile(filePath);
    if (ss) {
        ps_block_begin(ss, nullptr);
        return Create(ss);
    }
    return nullptr;
}

/*
bool nsVisualFactory2d::Parse(script_state_t *ss, nsVisualObject2d *object) {
    auto builder = GetBuilder(ps_block_name(ss));
    if (builder) {
        return builder->Parse(ss, object, this);
    }
    return false;
}
*/

nsVisualBuilder2d *nsVisualFactory2d::GetBuilder(const char *name) {
    if (!StrCheck(name)) {
        Log::Warning("Invalid layout builder name!");
        return nullptr;
    }

    auto it = _builders.find(name);
    if (it != _builders.end()) {
        return (*it).second;
    }

    Log::Warning("Layout builder not found: %s", name);
    return nullptr;
}


