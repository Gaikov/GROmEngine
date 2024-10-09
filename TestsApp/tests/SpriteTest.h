//
// Created by Roman Gaikov on 2019-08-15.
//

#pragma once
#include "TestCommon.h"
#include "nsLib/math/Transform2.h"
#include "engine/renderer/sprites/Sprite9SliceDesc.h"

class SpriteTest final : public BaseFunctionalTest
{
public:
	bool Init() override;

	void Loop(float deltaTime) override;

	void Draw() override;

	void Release() override;

private:
	ITexture *_tex = nullptr, *_tex1 = nullptr;

	float _angle = 0;
	float _scaleAngle = 0;
	float _scale = 1;

	nsTransform2 _origin1, _origin2;
	bool _update = true;

    nsTransform2 _origin3;
    nsSprite9SliceDesc _slice9;

	void DrawSprite(ITexture *t, const nsColor &c = nsColor::white);
	void OnKeyDown(int key, bool rept) override;
};