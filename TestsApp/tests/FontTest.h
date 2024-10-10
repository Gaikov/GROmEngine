//
// Created by Raman Haikou on 2019-08-12.
//

#pragma once
#include "TestCommon.h"
#include "Engine/Font.h"

class FontTest : public BaseFunctionalTest
{
public:
	bool Init() override;

	void Loop(float deltaTime) override;

	void Draw() override;

	void Release() override;

private:
	nsFont _font;
};