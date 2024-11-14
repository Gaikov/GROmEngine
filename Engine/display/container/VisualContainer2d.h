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
    typedef std::function<bool(nsVisualObject2d*)>  tChildCallback;

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

    template <class TClass>
    void GetChildrenRecursive(std::vector<TClass*> &list) {
        for (auto child : _children) {
            if (auto obj = dynamic_cast<TClass*>(child)) {
                list.push_back(obj);
            }

            if (auto container = dynamic_cast<nsVisualContainer2d*>(child)) {
                container->GetChildrenRecursive<TClass>(list);
            }
        }
    }

    bool IterateRecursive(const tChildCallback &callback);

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