// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualFactory2d.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "VisualCreationContext.h"
#include "VisualBuilder2d.h"
#include "nsLib/SubSystem.h"

class nsVisualFactory2d : public nsVisualCreationContext2d, public nsSubSystem<nsVisualFactory2d> {
public:
    nsVisualFactory2d();

    nsVisualObject2d *Create(script_state_t *ss) override;
    nsVisualObject2d *Create(const char *filePath);
    //bool Parse(script_state_t *ss, nsVisualObject2d *object) override;

    nsVisualBuilder2d* GetBuilder(const char *name);

    template<class TBuilder>
    void RegisterBuilderWithName() {
        nsVisualBuilder2d::sp_t builder = new TBuilder();
        RegisterBuilder(TBuilder::NAME, builder);
    }

    void RegisterBuilder(const char *name, nsVisualBuilder2d::sp_t &builder);

private:
    std::map<std::string, nsVisualBuilder2d::sp_t>  _builders;
};