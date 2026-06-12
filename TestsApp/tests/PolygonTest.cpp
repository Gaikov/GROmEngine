//
// Created by Roman on 6/9/2026.
//

#include "PolygonTest.h"
#include "Engine/display/VisualSceneRender2d.h"
#include "Engine/display/factory/VisualFactory2d.h"
#include "Engine/utils/AppUtils.h"
#include "nsLib/log.h"

bool PolygonTest::Init() {
    nsVisualFactory2d factory;
    _sceneRoot = factory.Create("tests/polygons.layout");
    if (!_sceneRoot) {
        Log::Error("Failed to load polygons.layout");
        return false;
    }

    Log::Info("Loaded polygon scene");
    return true;
}

void PolygonTest::Loop(float deltaTime) {
}

void PolygonTest::Draw() {
    if (_sceneRoot) {
        _sceneRoot->origin.pos = nsAppUtils::GetClientSize() / 2;
        nsVisualSceneRender2d::DrawScene(_sceneRoot);
    }
}

void PolygonTest::Release() {
    delete _sceneRoot;
    _sceneRoot = nullptr;
}
