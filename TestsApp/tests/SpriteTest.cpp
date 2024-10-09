//
// Created by Roman Gaikov on 2019-08-15.
//

#include "SpriteTest.h"

#include "engine/KeyCodes.h"
#include "engine/Platform.h"

#include "nsLib/math/Matrix2.h"
#include "engine/renderer/sprites/SpriteDesc.h"

bool SpriteTest::Init()
{
	_tex = _device->TextureLoad("textures/menu/big_font_en_3_2.PNG");
	_tex1 = _device->TextureLoad("textures/sysfont.png");


    _origin3.pos = { 200, 200};
    _slice9.tex = _device->TextureLoad("default/ui/frame.png");
    _slice9.SetGrid(6, 6, 6, 6);
    _slice9.color = nsColor::green;

	_origin1.pos = nsVec2(200, 200);
	_origin2.pos = nsVec2(400, 200);
	_origin1.AddChild(&_origin2);

	return true;
}

void SpriteTest::Loop(const float deltaTime)
{
	int w, h;
	App_GetPlatform()->GetClientSize(w, h);
	_origin1.pos = nsVec2(static_cast<float>(w) / 2.0f, static_cast<float>(h) / 2);

	_angle += nsMath::ToRad(45) * deltaTime;
	_scaleAngle += nsMath::ToRad(200) * deltaTime;
	_scale = cosf(_scaleAngle) * 0.5f + 1;
}

void SpriteTest::Draw() {
	nsMatrix	m;

	if (_update) {
		_origin1.angle = _angle;
	}
	_origin1.GetWorld().ToMatrix3(m);
	_device->LoadMatrix(m);
	DrawSprite(_tex, nsColor::red);

	if (_update) {
		_origin2.angle = -_angle;
		_origin2.scale = nsVec2(_scale, 1);
	}
	_origin2.GetWorld().ToMatrix3(m);
	_device->LoadMatrix(m);
    DrawSprite(_tex1, nsColor::blue);

	m.Identity();
	_device->LoadMatrix(m);

    nsColor c = nsColor::white;
    c.a = 0.1f;
    DrawSprite(_tex1, c);
    
    _origin3.GetWorld().ToMatrix3(m);
    _device->LoadMatrix(m);

    _slice9.SetWidth(100 * (sinf(_angle) + 1));
    _slice9.SetHeight(100 * (cosf(_angle) + 1));

    _slice9.Draw(_device);
}

void SpriteTest::Release()
{

}

void SpriteTest::DrawSprite(ITexture *t, const nsColor &c)
{
	if (!t)
	{
		return;
	}

	int w, h;
	t->GetSize(w, h);

	nsSpriteDesc s;
	s.tex = t;
    s.color = c;
	s.size = nsVec2(float(w), float(h));

    s.Draw(_device);
}

void SpriteTest::OnKeyDown(const int key, const bool rept) {
	BaseFunctionalTest::OnKeyDown(key, rept);

	if (key == NS_KEY_SPACE) {
		_update = !_update;
	}
}
