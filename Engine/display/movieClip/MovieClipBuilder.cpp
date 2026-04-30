// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file MovieClipBuilder.cpp
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#include "MovieClipBuilder.h"
#include "MovieClip.h"
#include "Core/ParserUtils.h"
#include "RenManager.h"

nsVisualObject2d* nsMovieClipBuilder::CreateDefault() {
    return new nsMovieClip();
}

bool nsMovieClipBuilder::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    auto *mc = dynamic_cast<nsMovieClip*>(object);
    if (!mc) return false;

    if (!nsVisualBuilder2d::Parse(ss, object, context)) return false;

    const auto dev = nsRenDevice::Shared()->Device();

    // Parse renState
    auto assetPath = context->assetsContext->ParseAssetPath(ss, "renState");
    if (!assetPath.IsEmpty()) {
        if (const auto state = dev->StateLoad(assetPath)) {
            mc->renState = state;
        }
    }

    while (ps_var_begin(ss, "anim")) {
        nsAnimClip clip;

        if (ps_var_begin(ss, "name")) {
            clip.name = ps_var_str(ss);
            ps_var_next(ss);
        }

        if (ps_var_begin(ss, "fps")) {
            clip.fps = (float)ps_var_f(ss);
            ps_var_next(ss);
        }

        while (ps_var_begin(ss, "frame")) {
            nsSpriteDesc desc;

            assetPath = context->assetsContext->ParseAssetPath(ss, "texture");
            if (!assetPath.IsEmpty()) {
                if (const auto tex = dev->TextureLoad(assetPath, false)) {
                    desc.tex = tex;
                    desc.ResetSize();
                }
            }

            if (ps_var_begin(ss, "tex1")) {
                ps_var_2f(ss, desc.tex1);
                ps_var_next(ss);
            }

            if (ps_var_begin(ss, "tex2")) {
                ps_var_2f(ss, desc.tex2);
                ps_var_next(ss);
            }

            if (ps_var_begin(ss, "size")) {
                ps_var_2f(ss, desc.size);
                ps_var_next(ss);
            }

            if (ps_var_begin(ss, "pivot")) {
                ps_var_2f(ss, desc.center);
                ps_var_next(ss);
            }

            clip.frames.push_back(desc);
            ps_var_next(ss);
        }

        mc->AddAnim(clip);
        ps_var_next(ss);
    }

    return true;
}
