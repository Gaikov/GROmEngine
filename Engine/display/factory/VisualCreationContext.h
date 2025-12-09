// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualCreationContext.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Core/Parser.h"
#include "Core/ScriptSaver.h"
#include "Engine/display/VisualObject2d.h"

class nsVisualBuilder2d;

class nsVisualCreationContext2d {
public:
    nsVisualCreationContext2d();
    virtual ~nsVisualCreationContext2d() = default;

    //project/game folder (relative to application or absolute)
    nsFilePath assetsPath;

    virtual nsVisualObject2d *Create(script_state_t *ss) = 0;
    virtual nsVisualObject2d *Create(const char *filePath) = 0;
    virtual nsVisualBuilder2d* GetBuilder(const char *name) = 0;
    virtual nsVisualObject2d* CreateByID(const char *bindingId) = 0;

    nsFilePath ParseAssetPath(script_state_t *ss, const char *name) const;
    void SaveAssetPath(const nsScriptSaver &saver, const char *name, const nsFilePath &path) const;

    [[nodiscard]] nsString RelativeAssetPath(const nsFilePath &path) const;
};