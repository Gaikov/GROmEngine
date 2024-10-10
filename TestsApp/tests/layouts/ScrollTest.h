// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ScrollTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/test/BaseFunctionalTest.h"
#include "Engine/display/layouts/GroupLayout.h"

class nsScrollTest : public BaseFunctionalTest {
public:
    bool Init() override;
    void Release() override;
    void Loop(float deltaTime) override;
    void Draw() override;
protected:
    IUserInput *GetInput() override;

private:
    nsGroupLayout   _root;
};