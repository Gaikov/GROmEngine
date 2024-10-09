//
// Created by Roman on 07/09/2019.
//

#include "Sprite3dTest.h"

Sprite3dTest::Sprite3dTest() {
}

bool Sprite3dTest::Init()
{
	if (!BaseFunctional3dTest::Init())
	{
		return false;
	}

	_tex = g_renDev->TextureLoad("textures/hero_life.png");
    _state = g_renDev->StateLoad("scripts/rs/alpha_blend.txt");

	return true;
}

void Sprite3dTest::Draw()
{
	BaseFunctional3dTest::Draw();

	g_renDev->TextureBind(_tex);
    g_renDev->StateApply(_state);
	g_renDev->DrawSprite3D(nsVec3(), 0.5, 0.5, nsMath::ToRad(_angle));
}

void Sprite3dTest::Release()
{

}

void Sprite3dTest::Loop(float deltaTime)
{
	BaseFunctional3dTest::Loop(deltaTime);
	_angle += deltaTime * 180.0f;
}
