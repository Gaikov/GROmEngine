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

    nsColor drawColor = color;
    if (premultiplyAlpha) {
        drawColor.r *= drawColor.a;
        drawColor.g *= drawColor.a;
        drawColor.b *= drawColor.a;
    }

    dev->TextureBind( tex );
    dev->SetColor(drawColor);

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
    if (name) {
        if (ps_block_begin(ss, name)) {
            Parse(ss, assets);
            ps_block_end(ss);
        } else if (ps_var_begin(ss, name)) {
            const int flags = premultiplyAlpha ? TLF_PREMULTIPLY_ALPHA : TLF_NONE;
            tex = nsRenDevice::Shared()->Device()->TextureLoad(assets->assetsPath.ResolvePath(ps_var_str(ss)), false, TF_RGBA, flags);
            ResetSize();
        }
        return;
    }

    premultiplyAlpha = ps_get_f(ss, "premultiplyAlpha", premultiplyAlpha) != 0;
    const int flags = premultiplyAlpha ? TLF_PREMULTIPLY_ALPHA : TLF_NONE;

    if (auto t = assets->ParseTexture(ss, "texture", flags)) {
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
