//
// Created by Roman on 8/28/2024.
//

#pragma once
#include "Engine/display/VisualObject2d.h"
#include "Engine/input/InteractiveElement.h"
#include "nsLib/color.h"

class nsBaseButton : public nsVisualObject2d, public nsInteractiveElement {
public:
    typedef std::function<void()> handler_t;

    IRenState   *renState = nullptr;
    nsBaseButton();
    float       paddings = 5;
    float       minWidth = 0;
    bool        drawFrame = true;
    nsColor     frameColor = nsColor(0.4, 0.4, 0.4, 1);
    nsColor     colorUp = nsColor(0.05, 0.05, 0.05, 1);
    nsColor     colorOver = nsColor(0.15, 0.15, 0.15, 1);
    nsColor     colorDown = nsColor(0.3, 0.3, 0.3, 1);

    void SetClickHandler(const handler_t &handler);

    void Loop() override;
    void GetLocalBounds(nsRect &r) override;

protected:
    void DrawContent(const nsVisualContext2d &context) override;
    virtual void GetContentSize(nsVec2 &size) = 0;

    void OnClick() override;

    void OnPointerOver() override {}
    void OnPointerOut() override {}
    void OnPointerDown() override {}
    void OnPointerUp() override {}

private:
    handler_t   _handler;
public:
    bool HitTest(float x, float y) override;
};
