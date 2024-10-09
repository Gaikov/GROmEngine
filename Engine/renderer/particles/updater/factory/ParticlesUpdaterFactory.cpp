//
// Created by Roman on 9/30/2024.
//

#include "ParticlesUpdaterFactory.h"
#include "renderer/particles/updater/ParticlesMultiUpdater.h"
#include "renderer/particles/updater/ParticlesGravityUpdater.h"
#include "renderer/particles/updater/ParticlesSizeUpdater.h"
#include "renderer/particles/updater/ParticlesVelocityApplyUpdater.h"
#include "renderer/particles/updater/size/ParticlesSizeTimelineUpdater.h"
#include "renderer/particles/updater/ParticlesVelToAngleUpdater.h"
#include "renderer/particles/updater/ParticlesColorTimelineUpdater.h"

nsParticlesUpdaterFactory::nsParticlesUpdaterFactory() {
    RegisterWithName<nsParticlesGravityUpdater>();
    RegisterWithName<nsParticlesCompositeUpdater>();
    RegisterWithName<nsParticlesSizeUpdater>();
    RegisterWithName<nsParticlesVelocityApplyUpdater>();
    RegisterWithName<nsParticlesSizeTimelineUpdater>();
    RegisterWithName<nsParticlesVelToAngleUpdater>();
    RegisterWithName<nsParticlesColorTimelineUpdater>();
}

nsParticlesUpdater *nsParticlesUpdaterFactory::Parse(script_state_t *ss) {
    auto updater = Create(ps_block_name(ss));
    if (updater && updater->Parse(ss, this)) {
        return updater;
    }
    delete updater;
    return nullptr;
}

nsParticlesUpdater *nsParticlesUpdaterFactory::ParseList(script_state_t *ss) {
    if (ps_block_begin(ss, nsParticlesCompositeUpdater::NAME)) {
        auto list = Parse(ss);
        ps_block_end(ss);
        return list;
    } else {
        Log::Warning("no particle updaters specified!");
    }
    return nullptr;
}

