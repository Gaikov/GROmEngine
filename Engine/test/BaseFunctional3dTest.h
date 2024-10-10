//
// Created by Roman on 07/09/2019.
//

#pragma once
#include "BaseFunctionalTest.h"

#include "engine/RenManager.h"
#include "engine/RenAux.h"
#include "nsLib/MathTools.h"

class BaseFunctional3dTest : public BaseFunctionalTest
{
public:
	BaseFunctional3dTest();
	bool Init() override;
	void Loop(float deltaTime) override;
	void Draw() override;
	void Release() override;
protected:
	float       _angle;
};
