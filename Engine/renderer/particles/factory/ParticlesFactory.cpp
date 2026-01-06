//
// Created by Roman on 12/24/2025.
//


#include "ParticlesFactory.h"

#include "Core/ParseFile.h"
#include "Core/ParserUtils.h"
#include "Core/ScriptSaver.h"
#include "renderer/particles/ParticlesBehaviour.h"

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
    behaviour->renderer = _rendererFactory.Parse(ss, assetsContext.get());

    return behaviour;
}

bool nsParticlesFactory::SaveBehaviour(const nsParticlesBehaviour *behaviour, const char *path) {
    if (!StrCheck(path)) {
        Log::Warning("Invalid particles file path!");
        return false;
    }
    Log::Info("...saving particles behaviour: %s", path);

    nsScriptSaver ss(path);
    if (ss.IsValid()) {
        ss.VarInt("amountPerSecond", behaviour->amountPerSecond, 100);
        ss.VarFloat("spawnTime", behaviour->spawnTime, 0);

        _spawnerFactory.Save(&ss, behaviour->spawner);
        _updaterFactory.Save(&ss, behaviour->updater);
        _rendererFactory.Save(ss, behaviour->renderer, assetsContext.get());

        return true;
    }

    return false;
}
