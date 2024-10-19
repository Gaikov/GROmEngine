// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualBuilder2d.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Core/Parser.h"
#include "Engine/display/VisualObject2d.h"
#include "Core/SmartPtr.h"
#include "VisualCreationContext.h"
#include "nsLib/log.h"

class nsVisualBuilder2d {
public:
    typedef nsSmartPtr<nsVisualBuilder2d>   sp_t;
    virtual ~nsVisualBuilder2d() = default;

    virtual nsVisualObject2d* Create(script_state_t *ss, nsVisualCreationContext2d *context);
    virtual nsVisualObject2d* CreateDefault() = 0;
    virtual bool Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context);

    template <class TClass>
    TClass* Cast(nsVisualObject2d *object) {
        auto res = dynamic_cast<TClass*>(object);
        if (!res) {
            Log::Warning("Invalid visual class: %s", typeid(TClass).name());
        }

        return res;
    }

    static void ParseAnchor(script_state_t *ss, const char *name, nsLayoutAnchor &anchor);
};