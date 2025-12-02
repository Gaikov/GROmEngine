// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualFactory2d.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "DefaultVisualBuilder2d.h"
#include "VisualCreationContext.h"
#include "VisualBuilder2d.h"
#include "Core/ScriptSaver.h"
#include "nsLib/SubSystem.h"
#include "nsLib/log.h"

class nsVisualFactory2d : public nsVisualCreationContext2d, public nsSubSystem<nsVisualFactory2d> {
public:
    nsVisualFactory2d();

    nsVisualObject2d *Create(script_state_t *ss) override;
    nsVisualObject2d *Create(const char *filePath) override;

    bool Serialize(const char *filePath, nsVisualObject2d *object);

    nsVisualBuilder2d* GetBuilder(const char *name) override;

    template<class TBuilder>
    void RegisterBuilderWithName() {
        nsVisualBuilder2d::sp_t builder = new TBuilder();
        RegisterBuilder(TBuilder::NAME, builder);
    }

    void RegisterBuilder(const char *name, nsVisualBuilder2d::sp_t &builder);

    nsVisualObject2d *CreateByID(const char *bindingId) override;

    template <class TVisualClass>
    void BindClass(const char *id) {
        if (_bindings.find(id) != _bindings.end()) {
            Log::Warning("Visual id '%s' already bound!", id);
        } else {
            _bindings[id] = [] {
                return new TVisualClass();
            };
        }
    }

private:
    std::map<std::string, nsVisualBuilder2d::sp_t>  _builders;
    std::map<std::string, std::function<nsVisualObject2d*()>> _bindings;
    nsDefaultVisualBuilder2d _defaultBuilder;
};