//
// Created by Roman on 8/13/2024.
//

#pragma once
#include "BasePopup.h"
#include "Engine/display/text/TextArea.h"
#include "Engine/display/button/TextButton.h"

struct nsButtonInfo {
    nsString    label;
    nsTextButton::handler_t onClick;
};

class nsMessagePopup : public nsBasePopup {
public:
    typedef std::vector<nsButtonInfo> buttonsList_t;

    static void Show(const char *caption, const char *message, const buttonsList_t &buttons);
    void Close();

protected:
    nsVisualObject2d * CreateContent() override;
    void DestroyContent(nsVisualObject2d *content) override;

private:
    nsTextArea  *_message = nullptr;
    nsVisualContainer2d *_buttonsContainer;

    nsTextButton* CreateButton(const nsButtonInfo &info);
    void CreateButtons(const buttonsList_t &buttons);
};

