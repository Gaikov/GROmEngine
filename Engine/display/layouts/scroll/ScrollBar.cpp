//
// Created by Roman on 9/18/2024.
//

#include "ScrollBar.h"
#include "display/sprite/Sprite9Slice.h"
#include "ScrollThumb.h"
#include "nsLib/log.h"

nsScrollBar::nsScrollBar() :
        thumbPaddings(2),
        contentSize(300),
        pageSize(100),
        scrollPosition(0) {

    auto tex = _device->TextureLoad("default/ui/frame.png");

    auto back = new nsSprite9Slice();
    back->desc.tex = tex;
    back->desc.SetGrid(6, 6, 6, 6);
    back->desc.color = nsColor::gray;
    SetBackground(back);

    SetThumb(new nsScrollThumb());

    nsBaseLayout::SetWidth(16);

    _onScrollChanged = [this](const nsBaseEvent &) -> void {
        if (_thumb) {
            _thumb->origin.pos = {thumbPaddings, ComputeThumbPos()};
        }
        //Log::Info("scroll pos: %f", (float) scrollPosition);
    };
    scrollPosition.AddHandler(nsPropChangedName::CHANGED, _onScrollChanged);
    contentSize.AddHandler(nsPropChangedName::CHANGED, _onScrollChanged);
    pageSize.AddHandler(nsPropChangedName::CHANGED, _onScrollChanged);
    thumbPaddings.AddHandler(nsPropChangedName::CHANGED, _onScrollChanged);
}

nsScrollBar::~nsScrollBar() {
    if (_back) {
        _back->Destroy();
    }

    if (_thumb) {
        _thumb->Destroy();
    }
}

void nsScrollBar::DestroyChildren() {
    nsBaseLayout::DestroyChildren();
    _back = nullptr;
    _thumb = nullptr;
}

void nsScrollBar::ComputeChildSize(nsVisualObject2d *child) {
    auto width = GetContentWidth();
    auto height = GetContentHeight();

    if (child == _back) {
        _back->SetWidth(width);
        _back->SetHeight(height);
    }

    if (child == _thumb) {
        bool allowed = IsAllowed();
        _thumb->visible = allowed;
        if (allowed) {
            _thumb->SetWidth(width - thumbPaddings * 2);
            _thumb->SetHeight(ComputeThumbSize());
        }
    }
}

void nsScrollBar::ComputeChildPlacement(nsVisualObject2d *child) {
    if (child == _back) {
        _back->origin.pos = {0, 0};
    }

    if (child == _thumb) {
        _thumb->origin.pos = {thumbPaddings, ComputeThumbPos()};
    }
}

void nsScrollBar::SetBackground(nsVisualObject2d *back) {
    assert(back != nullptr);

    if (_back != back) {
        if (_back) {
            RemoveChild(_back);
            _back->Destroy();
        }

        _back = back;
        AddChild(_back);
    }
}

void nsScrollBar::SetThumb(nsVisualObject2d *thumb) {
    assert(thumb != nullptr);

    if (_thumb != thumb) {
        if (_thumb) {
            RemoveChild(_thumb);
            _thumb->Destroy();
        }

        _thumb = thumb;
        AddChild(_thumb);
    }
}

float nsScrollBar::ComputeThumbSize() {
    auto maxSize = GetContentHeight() - thumbPaddings * 2;
    return pageSize * maxSize / contentSize;
}

float nsScrollBar::ComputeThumbPos() {
    float thumbSize = ComputeThumbSize();

    auto maxScroll = contentSize - pageSize;
    float currScroll = std::min(maxScroll, (float) scrollPosition);
    float factor = currScroll / maxScroll;

    float minPos = thumbPaddings;
    float maxPos = GetContentHeight() - thumbPaddings - thumbSize;

    return nsMath::Lerp(minPos, maxPos, (1 - factor));
}

float nsScrollBar::ComputeScrollPos(float localPos) {
    float height = GetContentHeight();
    float maxPos = height - thumbPaddings - ComputeThumbSize();
    float pos = nsMath::Clamp(localPos - thumbPaddings, 0, maxPos);
    float factor = pos / maxPos;

    float maxScroll = contentSize - pageSize;
    return maxScroll * (1 - factor);
}

bool nsScrollBar::OnPointerDown(float x, float y, int pointerId) {
    if (!_down && nsVisualContainer2d::HitTest(x, y)) {
        _down = true;
        nsVec2 localPos = origin.ToLocal({x, y});

        auto thumbSize = ComputeThumbSize();
        auto thumbPos = _thumb->origin.pos->y;
        if (nsMath::InRange(localPos.y, thumbPos, thumbSize + thumbPos)) {
            _offset = thumbPos - localPos.y;
        } else {
            _offset = -ComputeThumbSize() / 2;
        }
        scrollPosition = ComputeScrollPos(localPos.y + _offset);
    }
    return nsVisualContainer2d::OnPointerDown(x, y, pointerId) || _down;
}

bool nsScrollBar::OnPointerMove(float x, float y, int pointerId) {
    if (_down && _pointerId == pointerId) {
        nsVec2 localPos = origin.ToLocal({x, y});
        scrollPosition = ComputeScrollPos(localPos.y + _offset);
    }

    return nsVisualContainer2d::OnPointerMove(x, y, pointerId) || _down;
}

bool nsScrollBar::OnPointerUp(float x, float y, int pointerId) {
    OnPointerCancel(pointerId);
    return nsVisualContainer2d::OnPointerUp(x, y, pointerId);
}

void nsScrollBar::OnPointerCancel(int pointerId) {
    if (_pointerId == pointerId) {
        _down = false;
    }
    nsVisualContainer2d::OnPointerCancel(pointerId);
}

