//
// Created by Roman on 07/09/2019.
//

#include "BaseFunctional3dTest.h"

BaseFunctional3dTest::BaseFunctional3dTest() :
	_angle(0)
{

}

bool BaseFunctional3dTest::Init()
{
	return true;
}

void BaseFunctional3dTest::Loop(float deltaTime)
{
	_angle += deltaTime * 50;
}

void BaseFunctional3dTest::Draw()
{
	g_renDev->ClearScene(CLR_CBUFF | CLR_ZBUFF);
	nsMatrix proj;
	proj.SetProjFovRH(nsMath::ToRad(30), 1.3f, 0.01f, 10.0f);
	g_renDev->LoadProjMatrix(proj);

	auto a = nsMath::ToRad(_angle);

	nsMatrix view;
	view.SetLookAtRH(nsVec3(cosf(a), 1, sinf(a)), nsVec3(), nsVec3(0, 1, 0));
	g_renDev->LoadViewMartix(view);

	nsVec3 v[4] = {
			nsVec3(-0.5, -0.5, -0.5),
			nsVec3(0.5, -0.5, -0.5),
			nsVec3(0.5, -0.5, 0.5),
			nsVec3(-0.5, -0.5, 0.5),
	};

	g_renDev->StateApply(nullptr);
	g_renDev->TextureBind(nullptr);

	for (auto v1 : v)
	{
		auto v2 = v1 + nsVec3(0, 1, 0);
		RX_DrawLine(v1, v2);
	}

}

void BaseFunctional3dTest::Release()
{

}

