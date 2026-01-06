//
// Created by Roman on 9/29/2024.
//

#include "ParticlesRendererFactory.h"
#include "RoundParticlesRenderer.h"
#include "Core/ParserUtils.h"
#include "ParticlesToonRenderer.h"

nsParticlesRendererFactory::nsParticlesRendererFactory() {
    RegisterWithName<nsParticlesRoundRenderer>();
    RegisterWithName<nsParticlesToonRenderer>();
}

nsParticlesRenderer *nsParticlesRendererFactory::Parse(script_state_t *ss, const nsVisualAssetsContext *context) {
    if (ps_block_begin(ss, "renderer")) {
        const auto name = ParseStrP(ss, "name", "__not_specified__");
        if (const auto builder = GetBuilder(name)) {
            if (const auto renderer = (*builder)()) {
                if (renderer->Parse(ss, context)) {
                    return renderer;
                }
                delete renderer;
            }
        }
    } else {
        Log::Warning("Particles renderer is not specified!");
    }
    return nullptr;
}

bool nsParticlesRendererFactory::Save(nsScriptSaver &saver, nsParticlesRenderer *renderer,
                                      const nsVisualAssetsContext *context) const {
    if (saver.BlockBegin("renderer")) {
        saver.VarString("name", renderer->GetName());
        renderer->Save(saver, context);
        saver.BlockEnd();
        return true;
    }
    return false;
}
