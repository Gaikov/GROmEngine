//
// Created by Raman Haikou on 2019-08-12.
//

#include "FontTest.h"
#include "engine/renderer/sprites/SpriteDesc.h"

bool FontTest::Init()
{
	return _font.Load("scripts/fnt_sys2.txt");
}

void FontTest::Loop(float deltaTime)
{

}

void FontTest::Draw()
{
	nsMatrix    proj;
	proj.SetOrthoDimRH(640, 480);
	g_renDev->LoadProjMatrix(proj);

	//nsString    text = "TEXT FONT works fine ! 123124";
	nsString    text = "LOADING";
	_font.Draw(text, nsVec2(100, 100), nsVec2(1, 1), nsColor::red, text.Length());

    nsSpriteDesc desc;

    desc.size = nsVec2(300, 300);
    desc.pos = nsVec2(300, 200);
    desc.color = nsColor::gray;

    RX_DrawSprite(desc);

    desc.tex = _font.tex_list[0];
    desc.color = nsColor::white;

    RX_DrawSprite(desc);

}

void FontTest::Release()
{

}
