//
// Created by Roman on 7/29/2024.
//

#include "SpriteDesc.h"

nsQuadsBuffer* nsSpriteDesc::_buff = nullptr;

nsSpriteDesc& nsSpriteDesc::ResetSize() {
    if (tex) {
        int w, h;
        tex->GetSize(w, h);
        size = nsVec2((float) w, (float) h);
    } else {
        size = nsVec2(1, 1);
    }
    return *this;
}

nsSpriteDesc& nsSpriteDesc::ComputeCenter() {
    center = nsVec2(size.x / 2.0f, size.y / 2.0f);
    return *this;
}

void nsSpriteDesc::Draw(IRenDevice *dev) const {
    if (!_buff) {
        _buff = new nsQuadsBuffer(dev, 1, false);
    }

    dev->TextureBind( tex );
    dev->SetColor(color);

    _buff->Clear();
    _buff->AddQuad(pos - center, {}, size, nsColor::white, tex1, tex2);
    _buff->Draw();
}

void nsSpriteDesc::GetBounds(nsRect &rect) const {
    nsVec2 p = pos - center;

    rect.x = p.x;
    rect.y = p.y;
    rect.width = size.x;
    rect.height = size.y;
}

void nsSpriteDesc::FlipY() {
    std::swap(tex1.y, tex2.y);
}

void nsSpriteDesc::FlipX() {
    std::swap(tex1.x, tex2.x);
}
