//
// Created by Roman on 5/30/2024.
//

#include "QuadsTest.h"
#include "engine/RenManager.h"
#include "engine/TimeFormat.h"

bool nsQuadsTest::Init() {
    _quads = new nsQuadsBuffer(g_renDev, 4);
    _tex = g_renDev->TextureLoad("default/ui/frame.png", false);

    return true;
}

void nsQuadsTest::Loop(float deltaTime) {

}

void nsQuadsTest::Draw() {
    g_renDev->TextureBind(_tex);

    _quads->Clear();

    _quads->AddQuad(nsVec2(100, 100), {},
                    nsVec2(200, 100),
                    nsColor::red,
                    nsVec2(0, 1), nsVec2(1, 0));

    _quads->AddQuad(nsVec2(200, 200), {},
                    nsVec2(300, 300),
                    nsColor::white,
                    nsVec2(0, 1), nsVec2(1, 0));

    static float angle = 0;
    angle += g_frameTime;

    _quads->AddQuad({400, 400},
                    {250, 150},
                    {500, 300},
                    nsColor::white,
                    {0, 1}, {1, 0}, angle);

    _quads->Draw();
}

void nsQuadsTest::Release() {
    delete _quads;
}
