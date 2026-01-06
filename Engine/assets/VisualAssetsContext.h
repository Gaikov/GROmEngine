// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualAssetsContext.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "nsLib/FilePath.h"
#include "Engine/renderer/particles/factory/ParticlesLoader.h"

class nsVisualAssetsContext {
public:
    typedef std::shared_ptr<nsVisualAssetsContext> sp_t;

    //project/game folder (relative to application or absolute)
    nsFilePath assetsPath;
    nsParticlesLoader *particlesLoader = nullptr;

    nsVisualAssetsContext();
    virtual ~nsVisualAssetsContext();

    nsFilePath ParseAssetPath(script_state_t *ss, const char *name) const;
    void SaveAssetPath(const nsScriptSaver &saver, const char *name, const nsFilePath &path) const;

    [[nodiscard]] nsString RelativeAssetPath(const nsFilePath &path) const;
};
