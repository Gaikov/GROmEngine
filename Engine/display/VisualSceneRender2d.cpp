//
// Created by Roman on 8/1/2024.
//

#include "VisualSceneRender2d.h"
#include "display/layouts/BaseLayout.h"
#include "display/layouts/LayoutUpdater.h"
#include "EngineContext.h"
#include "engine/display/context/VisualMaskManager.h"

void nsVisualSceneRender2d::DrawScene(nsVisualObject2d *sceneRoot) {
    auto &context = nsEngineContext::Shared()->GetContext2D();
    if (context.updateLayouts) {
        nsLayoutUpdater::UpdateLayouts(sceneRoot);
    }

    auto mm = context.GetMaskManager();
    mm->BeginScene();
    sceneRoot->DrawNode(context);
    mm->EndScene();
}