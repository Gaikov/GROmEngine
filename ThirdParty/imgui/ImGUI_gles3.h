// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ImGUI_gles3.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

class nsImGUI_gles3 {
public:
    virtual ~nsImGUI_gles3() = default;
    virtual bool Init(void *window);
    virtual void Shutdown();

    virtual void StartFrame();
    virtual void EndFrame();
};