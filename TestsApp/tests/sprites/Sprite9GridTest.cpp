//
// Created by Roman on 5/31/2024.
//

#include "Sprite9GridTest.h"
#include "engine/RenManager.h"
#include "engine/RenAux.h"

bool nsSprite9GridTest::Init() {
    _sprite.SetTexture(g_renDev->TextureLoad("textures/ui/text_field_frame.png", false));
    _sprite.SetGrid(25, 22, 25, 22);
    _sprite.SetGridWidth(10);
    _sprite.SetGridHeight(5);
    _sprite.color = nsColor::red;

    return true;
}

void nsSprite9GridTest::Loop(float deltaTime) {

}

void nsSprite9GridTest::Draw() {
    nsVec2 pos(100, 100);

    nsMatrix m;
    m.Identity();
    m.SetPos(nsVec3(pos.x, pos.y, 0));

    g_renDev->LoadMatrix(m);

    _sprite.Draw(g_renDev);

    m.Identity();
    g_renDev->LoadMatrix(m);

    RX_DrawLine(pos, nsVec2(pos.x + _sprite.GetWidth(), pos.y));
    RX_DrawLine(pos, nsVec2(pos.x, pos.y + _sprite.GetHeight()));
}

void nsSprite9GridTest::Release() {

}
