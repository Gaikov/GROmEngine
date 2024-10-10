#pragma once

#include "BaseButton.h"
#include "nsLib/headers.h"
#include "nsLib/color.h"
#include "nsLib/StrTools.h"
#include "nsLib/Vec2.h"
#include "Engine/display/VisualObject2d.h"

class nsTextButton : public nsBaseButton {
public:
    typedef std::function<void()> handler_t;

    nsFont      *font;
    nsString    label = "label";
    nsColor     labelColor = nsColor(0.7, 0.7, 0.7, 1);
    nsColor     labelDisabledColor = nsColor(0.7, 0.7, 0.7, 0.1);
    nsVec2      labelScale = nsVec2(0.5, 1);

    nsTextButton();

protected:
    void GetContentSize(nsVec2 &size) override;

public:
    void DrawContent(const nsVisualContext2d &context) override;
};
