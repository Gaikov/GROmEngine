// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ScrollTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "engine/test/BaseFunctionalTest.h"
#include "engine/display/layouts/GroupLayout.h"

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