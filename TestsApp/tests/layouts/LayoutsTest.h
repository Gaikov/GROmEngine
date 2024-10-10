// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file LayoutsTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/test/BaseFunctionalTest.h"
#include "Engine/display/text/TextLabel.h"
#include "Engine/display/layouts/BaseLayout.h"
#include "Engine/display/layouts/GroupLayout.h"

class nsLayoutsTest final : public BaseFunctionalTest {
public:
    bool Init() override;
    void Loop(float deltaTime) override;
    void Draw() override;
    void Release() override;

    nsTextLabel* CreateLabel(const char *text, nsVisualContainer2d *container);
    IUserInput *GetInput() override;

private:
    nsGroupLayout _root;

    void CreateGroupLayout();
    void CreateVGroupLayout();
    void CreateHGroupLayout();

};