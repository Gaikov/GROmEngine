//
// Created by Roman on 08/09/2019.
//

#include "PlaneSprite3dTest.h"

PlaneSprite3dTest::PlaneSprite3dTest() :
		_tex(nullptr)
{

}

bool PlaneSprite3dTest::Init()
{
	if (!BaseFunctional3dTest::Init())
	{
		return false;
	}

	_tex = g_renDev->TextureLoad("textures/ctrls/flash01.jpg");
	return _tex != nullptr;
}

void PlaneSprite3dTest::Draw()
{
	BaseFunctional3dTest::Draw();
	
	g_renDev->TextureBind(_tex);
	g_renDev->DrawPlaneSprite(nsVec3(-0.5, 0, 0), nsVec3(0.5, 0, 0), 0.1, 5);
}
