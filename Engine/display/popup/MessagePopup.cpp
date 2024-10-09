//
// Created by Roman on 8/13/2024.
//

#include "MessagePopup.h"
#include "PopupManager.h"
#include "display/layouts/VGroupLayout.h"
#include "display/layouts/HGroupLayout.h"

nsVisualObject2d *nsMessagePopup::CreateContent() {
    auto group = new nsVGroupLayout();
    group->boundsType = CHILDREN;
    group->gap = 20;

    _message = new nsTextArea();
    _message->hAlign = nsAlign::CENTER;
    _message->xCenter.SetNumber(0);
    _message->origin.scale = {0.5, 1};
    group->AddChild(_message);

    auto buttons = new nsHGroupLayout();
    buttons->boundsType = CHILDREN;
    buttons->gap = 20;
    buttons->xCenter.SetNumber(0);
    _buttonsContainer = buttons;
    group->AddChild(buttons);

    return group;
}

void nsMessagePopup::DestroyContent(nsVisualObject2d *content) {
    content->Destroy();
}

void nsMessagePopup::Show(const char *caption, const char *message, const buttonsList_t &buttons) {
    auto popup = nsPopupManager::Shared()->CreatePopup<nsMessagePopup>();
    popup->_caption->text = caption;
    popup->_message->SetText(message);
    popup->CreateButtons(buttons);
}

void nsMessagePopup::Close() {
    nsPopupManager::Shared()->DestroyPopup(this);
}

nsTextButton *nsMessagePopup::CreateButton(const nsButtonInfo &info) {
    const auto button = new nsTextButton();
    button->label = info.label;
    button->SetClickHandler([this, info]() -> void {
        if (info.onClick) {
            info.onClick();
        }
        Close();
    });
    return button;
}

void nsMessagePopup::CreateButtons(const nsMessagePopup::buttonsList_t &buttons) {
    if (!buttons.empty()) {
        for (auto &info: buttons) {
            _buttonsContainer->AddChild(CreateButton(info));
        }
    } else {
        _buttonsContainer->AddChild(CreateButton({"Ok"}));
    }
}
