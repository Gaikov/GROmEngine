//
// Created by Roman on 07/09/2019.
//

#include "Sprite3dTest.h"

bool Sprite3dTest::Init()
{
	if (!BaseFunctional3dTest::Init())
	{
		return false;
	}

	_tex = _device->TextureLoad("tests/mask.png");
    _state = _device->StateLoad("default/rs/alpha_blend.txt");

	return true;
}

void Sprite3dTest::Draw()
{
	BaseFunctional3dTest::Draw();

	_device->TextureBind(_tex);
    _device->StateApply(_state);
    _device->SetColor(nsColor::white);
	_device->DrawSprite3D(nsVec3(), 0.5, 0.5, nsMath::ToRad(_angle));
}

void Sprite3dTest::Release()
{

}

void Sprite3dTest::Loop(float deltaTime)
{
	BaseFunctional3dTest::Loop(deltaTime);
	_angle += deltaTime * 180.0f;
}
