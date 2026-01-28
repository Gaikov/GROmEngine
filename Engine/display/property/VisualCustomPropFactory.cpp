//
// Created by Roman on 1/28/2026.
//

#include "VisualCustomPropFactory.h"

#include "Core/ParserUtils.h"
#include "Core/serialization/SerializeUtils.h"
#include "Core/serialization/var/BoolVar.h"
#include "Core/serialization/var/FloatVar.h"
#include "Core/serialization/var/StringVar.h"
#include "nsLib/structs/StructDef.h"

DEFINE_STATIC_MAP(nsVisualPropType, nsVisualCustomProp::Type, {
                      {"none", nsVisualCustomProp::NONE},
                      {"string", nsVisualCustomProp::STRING},
                      {"enum", nsVisualCustomProp::ENUM},
                      {"bool", nsVisualCustomProp::BOOL},
                      {"number", nsVisualCustomProp::NUMBER},
                  });

nsVisualCustomPropFactory::nsVisualCustomPropFactory() {
    Register<nsFloatVar>(nsVisualCustomProp::NUMBER, 0.0f);
    Register<nsBoolVar>(nsVisualCustomProp::BOOL, false);
    Register<nsStringVar>(nsVisualCustomProp::STRING, "");
    Register<nsStringVar>(nsVisualCustomProp::ENUM, "");
}

nsVisualCustomProp::sp_t nsVisualCustomPropFactory::Create(const nsVisualCustomProp::Type type) {
    const auto it = _builders.find(type);
    assert(it != _builders.end());
    const auto value = it->second();
    return std::shared_ptr<nsVisualCustomProp>(new nsVisualCustomProp(type, value));
}

void nsVisualCustomPropFactory::Serialize(nsScriptSaver &saver, const nsVisualCustomProp::sp_t &prop) {
    saver.VarString("type", nsVisualPropType::GetName(prop->GetType()));
    nsSerializeUtils::SerializeVar(saver, "value", prop->GetValue());
}

nsVisualCustomProp::sp_t nsVisualCustomPropFactory::Deserialize(script_state_t *ss) {
    const nsVisualCustomProp::Type type = nsVisualPropType::Get(ParseStrP(ss, "type"));
    const nsVisualCustomProp::sp_t prop = Create(type);
    if (prop) {
        nsSerializeUtils::DeserializeVar(ss, "value", prop->GetValue());
    }

    return prop;
}