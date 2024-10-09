// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualRenderManager2d.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "VisualObject2d.h"

class nsVisualSceneRender2d final {
public:
    static void DrawScene(nsVisualObject2d *sceneRoot);
};