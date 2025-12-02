//
// Created by Roman on 10/14/2024.
//

#include "VisualFactory2d.h"
#include "nsLib/log.h"
#include "Core/ParseFile.h"
#include "display/sprite/SpriteBuilder.h"
#include "display/text/TextLabelBuilder.h"
#include "display/container/VisualContainerBuilder.h"
#include "LayoutRefBuilder.h"
#include "display/particles/VisualParticlesBuilder.h"
#include "display/group/GroupLayoutBuilder.h"
#include "display/group/vertical/VGroupLayoutBuilder.h"
#include "display/button/TextButtonBuilder.h"
#include "display/helpers/VisualAnchorBuilder.h"
#include "CustomVisual.h"
#include "display/graphics/VirtualCircleBuilder.h"
#include "display/graphics/VisualRectBuilder.h"
#include "display/group/horizontal/HGroupLayoutBuilder.h"
#include "display/button/ImageButtonBuilder.h"
#include "display/sprite/Sprite9SliceBuilder.h"
#include "display/text/TextAreaBuilder.h"

nsVisualFactory2d::nsVisualFactory2d() {
    RegisterBuilderWithName<nsSpriteBuilder>();
    RegisterBuilderWithName<nsTextLabelBuilder>();
    RegisterBuilderWithName<nsVisualContainerBuilder>();
    RegisterBuilderWithName<nsLayoutRefBuilder>();
    RegisterBuilderWithName<nsVisualParticlesBuilder>();
    RegisterBuilderWithName<nsGroupLayoutBuilder>();
    RegisterBuilderWithName<nsVGroupLayoutBuilder>();
    RegisterBuilderWithName<nsHGroupLayoutBuilder>();
    RegisterBuilderWithName<nsTextButtonBuilder>();
    RegisterBuilderWithName<nsTextAreaBuilder>();
    RegisterBuilderWithName<nsVisualAnchorBuilder>();
    RegisterBuilderWithName<nsVirtualCircleBuilder>();
    RegisterBuilderWithName<nsVisualRectBuilder>();
    RegisterBuilderWithName<nsImageButtonBuilder>();
    RegisterBuilderWithName<nsSprite9SliceBuilder>();
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
    if (!builder) {
        builder = &_defaultBuilder;
    }

    if (auto object = builder->Create(ss, this)) {
        if (builder->Parse(ss, object, this)) {
            if (const auto custom = dynamic_cast<nsCustomVisual *>(object)) {
                if (!custom->ParseCustomProps(ss)) {
                    object->Destroy();
                    object = nullptr;
                }
            }

            return object;
        }

        object->Destroy();
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
    Log::Warning("can't create layout: %s", filePath);
    return nullptr;
}

bool nsVisualFactory2d::Serialize(const char *filePath, nsVisualObject2d *object) {
    Log::Info("serializing layout: %s", filePath);
    nsScriptSaver ss(filePath);
    if (!ss.IsValid()) {
        return false;
    }

    if (const auto builder = GetBuilder(object->GetType())) {
        builder->Serialize(ss, object, this);
        return true;
    }

    return false;
}

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

nsVisualObject2d *nsVisualFactory2d::CreateByID(const char *bindingId) {
    if (!StrCheck(bindingId)) {
        return nullptr;
    }

    auto it = _bindings.find(bindingId);
    if (it != _bindings.end()) {
        return it->second();
    }
    return nullptr;
}
