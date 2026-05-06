// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file MovieClipBuilder.cpp
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#include "MovieClipBuilder.h"
#include "MovieClip.h"
#include "Core/ParserUtils.h"
#include "RenManager.h"
#include "Engine/assets/VisualAssetsContext.h"

nsVisualObject2d *nsMovieClipBuilder::CreateDefault() {
    return new nsMovieClip();
}

bool nsMovieClipBuilder::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    auto *mc = dynamic_cast<nsMovieClip *>(object);
    if (!mc) return false;

    if (!nsVisualBuilder2d::Parse(ss, object, context)) return false;

    const auto dev = nsRenDevice::Shared()->Device();

    auto assetPath = context->assetsContext->ParseAssetPath(ss, "renState");
    if (!assetPath.IsEmpty()) {
        if (const auto state = dev->StateLoad(assetPath)) {
            mc->renState = state;
        }
    }

    if (ps_block_begin(ss, "anim")) {
        do {
            auto clip = std::make_shared<nsAnimClip>();

            if (ps_var_begin(ss, "name")) {
                clip->name = ps_var_str(ss);
            }

            if (ps_var_begin(ss, "fps")) {
                clip->fps = ps_var_f(ss);
            }

            if (ps_block_begin(ss, "frame")) {
                do {
                    auto frame = std::make_shared<nsSpriteDesc>();

                    if (const auto tex = context->assetsContext->ParseTexture(ss, "texture")) {
                        frame->tex = tex;
                        frame->ResetSize();
                    }

                    if (ps_var_begin(ss, "tex1")) { ps_var_2f(ss, frame->tex1); }
                    if (ps_var_begin(ss, "tex2")) { ps_var_2f(ss, frame->tex2); }
                    if (ps_var_begin(ss, "size")) { ps_var_2f(ss, frame->size); }
                    if (ps_var_begin(ss, "pivot")) { ps_var_2f(ss, frame->center); }

                    clip->frames.push_back(frame);
                } while (ps_block_next(ss));
                ps_block_end(ss);
            }

            mc->AddAnim(clip);
        } while (ps_block_next(ss));
        ps_block_end(ss);
    }

    // Apply after all anims are loaded so SetAnim can find the clip
    if (ps_var_begin(ss, "defaultAnim")) {
        mc->defaultAnim = ps_var_str(ss);
    }

    return true;
}

bool nsMovieClipBuilder::SerializeProps(nsScriptSaver &saver, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::SerializeProps(saver, o, context)) {
        return false;
    }

    auto *mc = Cast<nsMovieClip>(o);
    if (!mc) {
        return false;
    }

    auto dev = nsRenDevice::Shared()->Device();

    if (mc->renState) {
        context->assetsContext->SaveAssetPath(saver, "renState", dev->StateGetPath(mc->renState));
    }

    if (!mc->defaultAnim.GetValue().empty()) {
        saver.VarString("defaultAnim", mc->defaultAnim.GetValue().c_str());
    }

    for (const auto &clip : mc->anims) {
        if (!saver.BlockBegin("anim")) {
            continue;
        }

        saver.VarString("name", clip->name.c_str());
        saver.VarFloat("fps", clip->fps, 10.0f);

        for (const auto &frame : clip->frames) {
            if (!saver.BlockBegin("frame")) {
                continue;
            }

            if (frame->tex) {
                context->assetsContext->SaveAssetPath(saver, "texture", dev->TextureGetPath(frame->tex));
            }

            saver.VarFloat2("tex1", frame->tex1, nsVec2());
            saver.VarFloat2("tex2", frame->tex2, nsVec2(1, 1));
            saver.VarFloat2("size", frame->size, nsVec2());
            saver.VarFloat2("pivot", frame->center, nsVec2());

            saver.BlockEnd();
        }

        saver.BlockEnd();
    }

    return true;
}
