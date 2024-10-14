//
// Created by Raman Haikou on 2019-08-12.
//

#include "FontTest.h"
#include "Engine/renderer/sprites/SpriteDesc.h"
#include "Engine/renderer/font/FontsCache.h"

bool FontTest::Init()
{
    auto cache = nsFontsCache::Shared();
    _font2 = cache->LoadFont("tests/fonts/bmfont.fnt");
	_font = cache->LoadFont("default/sysfont.txt");
    return _font && _font2;
}

void FontTest::Loop(float deltaTime)
{

}

void FontTest::Draw()
{
	nsString    text = "TEXT FONT works fine ! 123124,_;{}?/$#";
	//nsString    text = "LOADING";
    nsRect  rect;
    nsVec2 size;
    nsVec2 pos = {100, 100};
	_font->Draw(text, nsVec2(100, 100), nsVec2(1, 1), nsColor::red, text.Length());
    _font->GetSize(text, size);

    rect.SetPos(pos);
    rect.SetSize(size);
    nsGizmos::DrawRect(rect, nsColor::white);

    //RX_DrawLine(pos, pos + nsVec2(1000, 0));

    nsSpriteDesc desc;

    desc.size = nsVec2(300, 300);
    desc.pos = nsVec2(300, 200);
    desc.color = nsColor::gray;

    RX_DrawSprite(desc);

    desc.tex = _font->GetPage(0);
    desc.color = nsColor::white;

    RX_DrawSprite(desc);

    pos = {100, 600};
    _font2->Draw(text, pos, nsVec2(1, 1), nsColor::blue, text.Length());
    _font2->GetSize(text, size);

    rect.SetPos(pos);
    rect.SetSize(size);
    nsGizmos::DrawRect(rect, nsColor::white);
}

void FontTest::Release()
{

}
