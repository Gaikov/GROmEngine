// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file LayoutsTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "engine/test/BaseFunctionalTest.h"
#include "engine/display/text/TextLabel.h"
#include "engine/display/layouts/BaseLayout.h"
#include "engine/display/layouts/GroupLayout.h"

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