//
// Created by Roman on 8/10/2024.
//

#include "TextArea.h"
#include "RenManager.h"
#include "RenAux.h"

nsTextArea::nsTextArea() {
    _font = nsRenDevice::Shared()->Font();
}

void nsTextArea::SetText(const char *text) {
    _validMetrics = false;
    _lines.clear();
    if (!StrCheck(text)) {
        return;
    }

    nsString line;
    auto ch = text;
    while (*ch) {
        if (*ch == '\n') {
            _lines.push_back({line});
            line = "";
        } else {
            line.Append(*ch);
        }
        ch++;
    }
    if (line.Length()) {
        _lines.push_back({line});
    }
}

void nsTextArea::ValidateMetrics() {
    if (!_validMetrics) {
        _validMetrics = true;
        //Log::Info("validating text metrics...");

        _bounds = {0, 0, 1, 1};

        if (_font) {
            float yPos = 0;
            for (auto &i: _lines) {
                _font->GetBounds(i.text, i.bounds);
                i.bounds.y = yPos;
                _bounds += i.bounds;
                yPos -= (i.bounds.height + _lineSpace);
            }
        }
    }
}

void nsTextArea::GetLocalBounds(nsRect &bounds) {
    ValidateMetrics();
    bounds = _bounds;
}

void nsTextArea::Loop() {

}

void nsTextArea::DrawContent(const nsVisualContext2d &context) {
    ValidateMetrics();

    if (_font) {
        nsVec2 scale(1, 1);
        for (auto &i: _lines) {
            float x = nsAlign::Compute(hAlign, i.bounds.width, _bounds.width);
            nsVec2 pos = {x, i.bounds.y};
            _font->Draw(i.text, pos, scale, color);
        }
    }

    if (drawFrame) {
        nsGizmos::DrawRect(_bounds, nsColor::darkGray);
    }
}

void nsTextArea::SetLineSpace(float space) {
    if (_lineSpace != space) {
        _lineSpace = space;
        _validMetrics = false;
    }
}

void nsTextArea::SetFont(nsFont *font) {
    if (font != _font) {
        _font = font;
        _validMetrics = false;
    }
}
