//
// Created by Roman on 10/15/2024.
//

#include "VisualParticlesBuilder.h"
#include "VisualParticles.h"
#include "renderer/particles/ParticlesManager.h"
#include "Core/ParserUtils.h"

nsVisualObject2d *nsVisualParticlesBuilder::CreateDefault() {
    return new nsVisualParticles();
}

bool nsVisualParticlesBuilder::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, o, context)) {
        return false;
    }

    const auto p = Cast<nsVisualParticles>(o);
    if (!p) {
        return false;
    }

    auto &system = p->GetSystem();
    const auto path = context->ParseAssetPath(ss, "source");
    if (!path.IsEmpty()) {
        auto b = nsParticlesManager::Shared()->LoadParticles(path);
        if (b) {
            system.behaviour = b;
        }
    }

    system.spawnEnabled = ParseBool(ss, "spawn", system.spawnEnabled);

    if (ps_var_begin(ss, "space")) {
        const auto space = ps_var_str(ss);
        if (StrEqual(space, "global")) {
            p->space = nsVisualParticles::GLOBAL;
        } else if (StrEqual(space, "parent")) {
            p->space = nsVisualParticles::PARENT;
        } else {
            p->space = nsVisualParticles::LOCAL;
        }
    }

    return true;
}

bool nsVisualParticlesBuilder::SerializeProps(nsScriptSaver &saver, nsVisualObject2d *o,
    nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::SerializeProps(saver, o, context)) {
        return false;
    }

    const auto p = Cast<nsVisualParticles>(o);
    const auto &system = p->GetSystem();
    if (system.behaviour) {
        const nsFilePath path = nsParticlesManager::Shared()->GetParticlesPath(system.behaviour);
        context->SaveAssetPath(saver, "source", path);
    }

    saver.VarBool("spawn", system.spawnEnabled, true);

    if (p->space == nsVisualParticles::GLOBAL) {
        saver.VarString("space", "global");
    } else if (p->space == nsVisualParticles::PARENT) {
        saver.VarString("space", "parent");
    }

    return true;
}
