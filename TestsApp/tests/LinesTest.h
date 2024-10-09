//
// Created by Roman on 11/08/2019.
//

#pragma once
#include "TestCommon.h"

class LinesTest : public BaseFunctionalTest
{
	bool Init() override;
	void Loop(float deltaTime) override;
	void Draw() override;
	void Release() override;
};
