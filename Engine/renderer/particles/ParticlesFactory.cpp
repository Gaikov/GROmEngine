//
// Created by Roman on 12/24/2025.
//

#include "ParticlesFactory.h"

#include "Core/ParseFile.h"
#include "Core/ParserUtils.h"

nsParticlesBehaviour * nsParticlesFactory::LoadBehaviour(const char *path) {
    if (!StrCheck(path)) {
        Log::Warning("Invalid particles file path");
        return nullptr;
    }
    Log::Info("...loading particles behaviour: %s", path);

    nsParseFile pf;
    const auto ss = pf.BeginFile(path);
    if (!ss) {
        return nullptr;
    }

    auto spawners = _spawnerFactory.ParseList(ss);
    if (!spawners) {
        return nullptr;
    }

    const auto behaviour = new nsParticlesBehaviour();

    behaviour->amountPerSecond = static_cast<int>(ParseFloat(ss, "amountPerSecond", 100));
    behaviour->spawnTime = ParseFloat(ss, "spawnTime", 0);
    behaviour->spawner = spawners;
    behaviour->updater = _updaterFactory.ParseList(ss);
    behaviour->renderer = _rendererFactory.Parse(ss);

    return behaviour;
}
