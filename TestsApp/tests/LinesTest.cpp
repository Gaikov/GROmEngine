//
// Created by Roman on 11/08/2019.
//

#include "LinesTest.h"

bool LinesTest::Init()
{
	return true;
}

void LinesTest::Loop(float deltaTime)
{

}

void LinesTest::Draw()
{
	nsMatrix    proj;
	proj.SetOrthoDimRH(640, 480);
	g_renDev->LoadProjMatrix(proj);

	RX_DrawLine(nsVec2(10, 0), nsVec2(10,300), nsColor::red);
	RX_DrawLine(nsVec2(100, 0), nsVec2(100,300), nsColor::green);
	RX_DrawLine(nsVec2(200, 0), nsVec2(200,300), nsColor::blue);
}

void LinesTest::Release()
{

}
