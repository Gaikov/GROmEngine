//
// Created by Roman on 8/21/2024.
//

#pragma once
#include "Engine/display/layouts/VGroupLayout.h"
#include "nsLib/SubSystem.h"
#include "soft/SoftKeyButtons.h"

class nsSoftKeyboard : public nsSubSystem<nsSoftKeyboard>, public ISoftKeyHandler {
public:
    enum {MAX_WIDTH = 400, MAX_HEIGHT = 170};

    nsVisualObject2d* GetRoot();
    void UpdateScale(float screenWidth, float screenHeight);
    void SetScale(float s);
    bool IsActive();
    void Activate(bool active);
    void SetHandler(IKeyboardInput *handler);

protected:
    bool OnInit() override;
    void OnRelease() override;

    void OnChar(char ch) override;

    void OnKeyDown(int keyCode) override;

private:
    IKeyboardInput *_handler = nullptr;
    nsVGroupLayout  *_root = nullptr;
    std::vector<nsLetterKeyButton*> _letters;
    bool _caps = false;
};
