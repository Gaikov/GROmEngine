//
// Created by Roman on 9/27/2024.
//

#include "ParticlesSpawnerFactory.h"
#include "renderer/particles/spawner/ParticlesAngleSpawner.h"
#include "Engine/renderer/particles/spawner/position/ParticlesCircleSpawner.h"
#include "renderer/particles/spawner/ParticlesLifeSpawner.h"
#include "renderer/particles/spawner/ParticlesMultiSpawner.h"
#include "Engine/renderer/particles/spawner/position/ParticlesPolygonSpawner.h"
#include "renderer/particles/spawner/velocity/ParticlesRadialVelocitySpawner.h"
#include "renderer/particles/spawner/ParticlesSizeSpawner.h"
#include "renderer/particles/spawner/velocity/ParticlesVectorVelSpawner.h"
#include "renderer/particles/spawner/velocity/ParticlesRightVelSpawner.h"
#include "renderer/particles/spawner/velocity/ParticlesMultiDirectionSpawner.h"

nsParticlesSpawnerFactory::nsParticlesSpawnerFactory() {
    RegisterWithName<nsParticlesAngleSpawner>();
    RegisterWithName<nsParticlesCircleSpawner>();
    RegisterWithName<nsParticlesLifeSpawner>();
    RegisterWithName<nsParticlesMultiSpawner>();
    RegisterWithName<nsParticlesPolygonSpawner>();
    RegisterWithName<nsParticlesRadialVelSpawner>();
    RegisterWithName<nsParticlesSizeSpawner>();
    RegisterWithName<nsParticlesVectorVelSpawner>();
    RegisterWithName<nsParticlesEdgesSpawner>();
    RegisterWithName<nsParticlesRightVelSpawner>();
    RegisterWithName<nsParticlesMultiDirectionSpawner>();
}

nsParticlesSpawner *nsParticlesSpawnerFactory::Parse(script_state_t *ss) {
    auto s = Create(ps_block_name(ss));
    if (s && s->Parse(ss, this)) {
        return s;
    }

    delete s;
    return nullptr;
}

nsParticlesSpawner *nsParticlesSpawnerFactory::ParseList(script_state_t *ss) {
    if (ps_block_begin(ss, nsParticlesMultiSpawner::NAME)) {
        const auto list = Parse(ss);
        ps_block_end(ss);
        return list;
    } else {
        Log::Warning("no particle spawners specified!");
    }
    return nullptr;
}


