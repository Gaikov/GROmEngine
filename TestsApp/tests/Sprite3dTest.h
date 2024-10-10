//
// Created by Roman on 07/09/2019.
//

#pragma once
#include "TestCommon.h"

class Sprite3dTest : public BaseFunctional3dTest
{
public:
	bool Init() override;
	void Loop(float deltaTime) override;
	void Draw() override;
	void Release() override;

private:
	ITexture    *_tex = nullptr;
    IRenState   *_state = nullptr;
	float       _angle = 0;
};
