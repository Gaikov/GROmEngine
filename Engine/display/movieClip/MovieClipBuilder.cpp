// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file MovieClipBuilder.cpp
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#include "MovieClipBuilder.h"
#include "MovieClip.h"
#include "Core/ParserUtils.h"
#include "RenManager.h"

nsVisualObject2d *nsMovieClipBuilder::CreateDefault() {
    return new nsMovieClip();
}

bool nsMovieClipBuilder::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    auto *mc = dynamic_cast<nsMovieClip *>(object);
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

    if (ps_block_begin(ss, "anim")) {
        do {
            nsAnimClip clip;

            if (ps_var_begin(ss, "name")) {
                clip.name = ps_var_str(ss);
            }

            if (ps_var_begin(ss, "fps")) {
                clip.fps = (float) ps_var_f(ss);
            }

            if (ps_block_begin(ss, "frame")) {
                do {
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
                    }

                    if (ps_var_begin(ss, "tex2")) {
                        ps_var_2f(ss, desc.tex2);
                    }

                    if (ps_var_begin(ss, "size")) {
                        ps_var_2f(ss, desc.size);
                    }

                    if (ps_var_begin(ss, "pivot")) {
                        ps_var_2f(ss, desc.center);
                    }

                    clip.frames.push_back(desc);
                } while (ps_block_next(ss));
                ps_block_end(ss);
            }

            mc->AddAnim(clip);
        } while (ps_block_next(ss));
        ps_block_end(ss);
    }

    return true;
}
