// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualContainer2d.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "VisualObject2d.h"
#include "engine/input/InteractiveElement.h"
#include "VisualContext2d.h"

class nsVisualContainer2d : public nsVisualObject2d, public IUserInput {
public:
    void Destroy() override;
    virtual void DestroyChildren();

    virtual void AddChild(nsVisualObject2d *obj);
    virtual void RemoveChild(nsVisualObject2d *obj);
    virtual void RemoveChildren();

    void Loop() override;
    void GetLocalBounds(nsRect &bounds) override;
    bool HitTest(float x, float y) override;

    const std::vector<nsVisualObject2d*>& GetChildren() const { return _children; }

    void DrawNode(const nsVisualContext2d &context) override;
    void DrawContent(const nsVisualContext2d &context) override;

protected:
    bool OnPointerUp(float x, float y, int pointerId) override;
    bool OnPointerDown(float x, float y, int pointerId) override;
    bool OnPointerMove(float x, float y, int pointerId) override;
    void OnPointerCancel(int pointerId) override;

    void OnKeyUp(int key) override;
    void OnKeyDown(int key, bool rept) override;
    void OnChar(char ch) override;
    void OnMouseWheel(float delta) override;

private:
    std::vector<nsVisualObject2d*>  _children;
    IUserInput  *_childOver = nullptr;
};