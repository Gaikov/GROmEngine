//
// Created by Roman on 07/09/2019.
//

#include "BaseFunctional3dTest.h"
#include "utils/AppUtils.h"

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
	_device->ClearScene(CLR_CBUFF | CLR_ZBUFF);
	nsMatrix proj;
    auto size = nsAppUtils::GetClientSize();

	proj.SetProjFovRH(nsMath::ToRad(30), size.x / size.y, 0.01f, 10.0f);
	_device->LoadProjMatrix(proj);

	auto a = nsMath::ToRad(_angle);

	nsMatrix view;
	view.SetLookAtRH(nsVec3(cosf(a), 1, sinf(a)), nsVec3(), nsVec3(0, 1, 0));
	_device->LoadViewMartix(view);

    struct Vert {
        nsVec3 v;
        nsColor c;
    };

	Vert v[4] = {
            { nsVec3(-0.5, -0.5, -0.5), nsColor::red },
            {nsVec3(0.5, -0.5, -0.5), nsColor::green },
            { nsVec3(0.5, -0.5, 0.5), nsColor::blue },
            { nsVec3(-0.5, -0.5, 0.5), nsColor::yellow }
	};

	_device->StateApply(nullptr);
	_device->TextureBind(nullptr);

	for (auto i : v)
	{
		auto v2 = i.v + nsVec3(0, 1, 0);
		RX_DrawLine(i.v, v2, i.c);
	}

}

void BaseFunctional3dTest::Release()
{

}

