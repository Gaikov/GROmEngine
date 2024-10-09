// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesTest.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "engine/test/BaseFunctionalTest.h"
#include "engine/renderer/particles/ParticleSystem.h"
#include "engine/display/layouts/GroupLayout.h"
#include "engine/display/text/TextLabel.h"

class nsParticlesTest : public BaseFunctionalTest {
public:
    bool Init() override;
    void Release() override;
    void Loop(float deltaTime) override;
    void Draw() override;

    bool OnPointerMove(float x, float y, int pointerId) override;
    void OnKeyDown(int key, bool rept) override;

    static nsTextLabel* CreateLabel(const char *text, nsVisualContainer2d *container);
    bool OnPointerDown(float x, float y, int pointerId) override;
    bool OnPointerUp(float x, float y, int pointerId) override;

private:
    nsGroupLayout       _root;
    nsParticleSystem    _system;

    nsVec2 _mousePos;
    bool _mouseDown = false;

    float  _angle = 0;
    float  _angleSpeed = 0;

    struct State {
        std::function<void()> init = []() -> void {};
        std::function<void()> loop = []() -> void {};
    };

    void AddState(const State &s) {
        _states.push_back(s);
    }

    std::vector<State> _states;
    int _currState = 0;

    std::vector<nsParticlesBehaviour*> _behaviours;
    int _currentBehaviour = 0;

    nsTextLabel *_labelCap = nullptr;
    nsTextLabel *_labelActive = nullptr;
    nsTextLabel *_labelPerSecond = nullptr;

};