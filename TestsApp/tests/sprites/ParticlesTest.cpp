//
// Created by Roman on 9/25/2024.
//

#include "ParticlesTest.h"
#include "Engine/display/VisualSceneRender2d.h"
#include "Engine/Platform.h"
#include "Engine/display/layouts/HGroupLayout.h"
#include "Engine/display/layouts/VGroupLayout.h"
#include "Engine/display/text/TextLabel.h"
#include "Engine/renderer/particles/renderer/RoundParticlesRenderer.h"
#include "Engine/TimeFormat.h"
#include "Engine/utils/AppUtils.h"
#include "Engine/KeyCodes.h"
#include "Engine/renderer/particles/factory/ParticlesManager.h"

bool nsParticlesTest::Init() {

    auto m = nsParticlesManager::Shared();
    _behaviours.push_back(m->LoadParticles("tests/particles/circle.ggps"));
    _behaviours.push_back(m->LoadParticles("tests/particles/hline.ggps"));
    _behaviours.push_back(m->LoadParticles("tests/particles/triangle.ggps"));
    _behaviours.push_back(m->LoadParticles("tests/particles/cross.ggps"));
    _behaviours.push_back(m->LoadParticles("tests/particles/point.ggps"));
    _behaviours.push_back(m->LoadParticles("tests/particles/fire.ggps"));
    _behaviours.push_back(m->LoadParticles("tests/particles/blast.ggps"));
    _behaviours.push_back(m->LoadParticles("tests/particles/sparkle.ggps"));
    _behaviours.push_back(m->LoadParticles("tests/particles/color.ggps"));

    _currentBehaviour = static_cast<int>(_behaviours.size() - 1);
    _system.behaviour = _behaviours[_currentBehaviour];

    const auto panel = new nsHGroupLayout();
    panel->boundsType = nsBaseLayout::CHILDREN;
    panel->xMin.SetNumber(10);
    panel->yMax.SetNumber(10);
    _root.AddChild(panel);

    auto labels = new nsVGroupLayout();
    labels->boundsType = nsBaseLayout::CHILDREN;
    panel->AddChild(labels);

    auto values = new nsVGroupLayout();
    values->boundsType = nsBaseLayout::CHILDREN;
    panel->AddChild(values);

    CreateLabel("Capacity: ", labels);
    _labelCap = CreateLabel("", values);

    CreateLabel("Active Particles: ", labels);
    _labelActive = CreateLabel("", values);

    CreateLabel("Particles Per Second: ", labels);
    _labelPerSecond = CreateLabel("", values);

    _system.SetPosition(nsAppUtils::GetClientSize() / 2);

    AddState({
                     [this]() -> void {
                         _currentBehaviour = (_currentBehaviour + 1) % (int) _behaviours.size();
                         _system.behaviour = _behaviours[_currentBehaviour];
                         Log::Info("behaviour changed: %i", _currentBehaviour);
                     },
             });

    AddState({
                     [this]() -> void {
                         _angle = 0;
                     },
                     [this]() -> void {
                         _angle += g_frameTime * 5;

                         auto size = nsAppUtils::GetClientSize();
                         auto pos = size / 2;

                         pos.x += sinf(_angle) * (size.x / 2 - 100);

                         _system.MoveTo(pos);
                     }
             });

    AddState({
                     [this] {
                         _angle = 0;
                         _angleSpeed = M_PI;
                     },
                     [this]() {
                         _angle += g_frameTime * _angleSpeed;
                         _system.RotateTo(_angle);
                     }
             });

    AddState({
                     [this]() -> void {
                         _system.spawnEnabled = false;
                     }
             });

    return true;
}

void nsParticlesTest::Release() {
    _root.DestroyChildren();
}

void nsParticlesTest::Loop(float deltaTime) {
    _states[_currState].loop();

    auto numActive = _system.Update(deltaTime);

    _labelActive->SetInt(numActive);
    _labelCap->SetInt(nsParticlesPool::Shared()->GetCapacity());
    _labelPerSecond->SetInt(_behaviours[_currentBehaviour]->amountPerSecond);
}

void nsParticlesTest::Draw() {
    _device->LoadMatrix(nsMatrix::identity);
    _system.Draw();

    int w, h;
    App_GetPlatform()->GetClientSize(w, h);
    _root.SetWidth((float) w);
    _root.SetHeight((float) h);
    nsVisualSceneRender2d::DrawScene(&_root);
}

bool nsParticlesTest::OnPointerDown(float x, float y, int pointerId) {
    _mousePos = WindowToScreen({x, y});
    _mouseDown = true;
    return BaseFunctionalTest::OnPointerDown(x, y, pointerId);
}

bool nsParticlesTest::OnPointerUp(float x, float y, int pointerId) {
    _mouseDown = false;
    return BaseFunctionalTest::OnPointerUp(x, y, pointerId);
}

bool nsParticlesTest::OnPointerMove(float x, float y, int pointerId) {
    BaseFunctionalTest::OnPointerMove(x, y, pointerId);
    _mousePos = WindowToScreen({x, y});
    return true;
}

nsTextLabel *nsParticlesTest::CreateLabel(const char *text, nsVisualContainer2d *container) const {
    const auto label = new nsTextLabel();
    label->text = text;
    label->renState = _device->StateLoad("default/rs/gui_clamp.ggrs");
    label->origin.scale = {0.5, 1};
    container->AddChild(label);
    return label;
}

void nsParticlesTest::OnKeyDown(int key, bool rept, int mods) {
    BaseFunctionalTest::OnKeyDown(key, rept, mods);
    Log::Info("key down: %i", key);
    if (!rept) {
        if (key == NS_KEY_SPACE) {
            _currState = (_currState + 1) % (int) _states.size();
            _system.SetRotation(0);
            _system.SetPosition(nsAppUtils::GetClientSize() / 2);
        } else if (key == NS_KEY_ENTER) {
            if (!_system.spawnEnabled) {
                _system.Emit();
            }
        } else if (key == NS_KEY_KP_PLUS) {
            _angleSpeed += M_PI;
        } else if (key == NS_KEY_KP_MINUS) {
            _angleSpeed -= M_PI;
        } else if (key >= 49 && key < 59) {
            _currState = (key - 49) % (int) _states.size();
            _system.SetRotation(0);
            _system.SetPosition(nsAppUtils::GetClientSize() / 2);
            _system.spawnEnabled = true;
            _states[_currState].init();
        }
    }
}
