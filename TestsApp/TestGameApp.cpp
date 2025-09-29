//
// Created by Roman on 09/07/2019.
//

#include "tests/PlaneSprite3dTest.h"
#include "tests/SpriteTest.h"
#include "tests/LinesTest.h"
#include "tests/FontTest.h"
#include "tests/Sprite3dTest.h"
#include "TestGameApp.h"
#include "tests/QuadsTest.h"
#include "tests/sprites/Sprite9GridTest.h"
#include "tests/GlobalToLocalTest.h"
#include "tests/RenderTextureTest.h"
#include "tests/layouts/LayoutsTest.h"
#include "tests/rays/RayMarchingTest.h"
#include "tests/StencilMaskTest.h"
#include "tests/layouts/ScrollTest.h"
#include "tests/sprites/ParticlesTest.h"
#include "tests/collision/CollisionTest.h"

TestGameApp *g_gameApp = nullptr;

IGameApp *App_GetGame() {
    if (!g_gameApp) {
        g_gameApp = new TestGameApp();
    }

    return g_gameApp;
}

void TestGameApp::RegisterTests() {
    RegisterTest("lines", new LinesTest());
    RegisterTest("font", new FontTest());
    RegisterTest("sprite", new SpriteTest());
    RegisterTest("sprite3d", new Sprite3dTest());
    RegisterTest("plane3d", new PlaneSprite3dTest());
    RegisterTest("quads", new nsQuadsTest());
    RegisterTest("sprite9grid", new nsSprite9GridTest());
    RegisterTest("toLocal", new nsGlobalToLocalTest());
    RegisterTest("layouts", new nsLayoutsTest());
    RegisterTest("raymarch_shadows", new nsRayMarchingTest());
    RegisterTest("mask", new nsStencilMaskTest());
    RegisterTest("scroll", new nsScrollTest());
    RegisterTest("parts", new nsParticlesTest());
    RegisterTest("collision", new nsCollisionTest());
    RegisterTest("render_texture", new nsRenderTextureTest());

    Log::Info("info test message: %i", 1);
    Log::Warning("warning test message: %i", 2);
    Log::Error("error test message: %i", 3);
    Log::Debug("debug test message: %i", 4);
    Log::Debug("Test\n\nMultilines\nin the console");
}

void TestGameApp::Release() {
    BaseTestApp::Release();
    delete g_gameApp;
    g_gameApp = nullptr;
}
