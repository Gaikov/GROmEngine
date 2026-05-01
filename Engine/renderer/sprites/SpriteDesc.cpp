//
// Created by Roman on 7/29/2024.
//

#include "SpriteDesc.h"
#include "Engine/assets/VisualAssetsContext.h"
#include "Core/ParserUtils.h"
#include "RenManager.h"

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

//---------------------------------------------------------
// nsSpriteDesc::Parse:
//---------------------------------------------------------
void nsSpriteDesc::Parse(script_state_t *ss, nsVisualAssetsContext *assets, const char *name) {
    auto dev = nsRenDevice::Shared()->Device();

    if (name) {
        if (ps_block_begin(ss, name)) {
            Parse(ss, assets);
            ps_block_end(ss);
        } else if (ps_var_begin(ss, name)) {
            tex = dev->TextureLoad(ps_var_str(ss), false);
            ResetSize();
        }
        return;
    }

    if (auto t = assets->ParseTexture(ss, "texture")) {
        tex = t;
        ResetSize();
    }

    if (ps_var_begin(ss, "pivot")) {
        ps_var_2f(ss, center);
    }

    if (ps_var_begin(ss, "size")) {
        ps_var_2f(ss, size);
    }

    ParseColorExt(ss, "color", color);

    if (ps_var_begin(ss, "tex1")) {
        ps_var_2f(ss, tex1);
    }

    if (ps_var_begin(ss, "tilesX")) {
        tex2.x = ps_var_f(ss);
    }

    if (ps_var_begin(ss, "tilesY")) {
        tex2.y = ps_var_f(ss);
    }
}
