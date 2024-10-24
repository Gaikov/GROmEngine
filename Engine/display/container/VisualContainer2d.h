// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualContainer2d.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once


#include "Engine/input/InteractiveElement.h"
#include "Engine/display/VisualObject2d.h"

class nsVisualContainer2d : public nsVisualObject2d, public IUserInput {
public:
    void Destroy() override;
    virtual void DestroyChildren();

    nsVisualObject2d *GetChildById(const char *id);
    nsVisualObject2d *GetChildByIdRecursive(const char *id);

    virtual void AddChild(nsVisualObject2d *obj);
    virtual void RemoveChild(nsVisualObject2d *obj);
    virtual void RemoveChildren();

    void Loop() override;
    void GetLocalBounds(nsRect &bounds) override;
    bool HitTest(float x, float y) override;

    const std::vector<nsVisualObject2d *> &GetChildren() const { return _children; }

    void DrawNode(const nsVisualContext2d &context) override;
    void DrawContent(const nsVisualContext2d &context) override;

    void FindChildrenRecursive(std::function<bool(nsVisualObject2d *child)> pred, std::vector<nsVisualObject2d *> &result);

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
    std::vector<nsVisualObject2d *> _children;
    IUserInput *_childOver = nullptr;
};