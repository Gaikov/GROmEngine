//
// Created by Roman on 10/14/2024.
//

#include "VisualContainerBuilder.h"
#include "Engine/display/container/VisualContainer2d.h"
#include "nsLib/log.h"
#include "Core/ParserUtils.h"
#include <sstream>

nsVisualObject2d *nsVisualContainerBuilder::CreateDefault() {
    return new nsVisualContainer2d();
}

bool nsVisualContainerBuilder::Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::Parse(ss, object, context)) {
        return false;
    }

    auto container = Cast<nsVisualContainer2d>(object);
    if (!container) {
        return false;
    }

    if (ps_block_begin(ss, "children")) {
        if (ps_block_begin(ss, nullptr)) {
            do {
                auto child = context->Create(ss);
                if (child) {
                    container->AddChild(child);
                }
            } while (ps_block_next(ss));
            ps_block_end(ss);
        }
        ps_block_end(ss);
    }

    if (ps_block_begin(ss, "masking")) {
        do {
            auto maskId = ParseString(ss, "mask");
            auto mask = dynamic_cast<nsVisualMask*>(container->GetChildById(maskId));
            if (mask) {
                auto maskedId = ParseString(ss, "masked");
                auto masked = container->GetChildById(maskedId);
                if (masked) {
                    masked->AddMask(mask);
                } else {
                    Log::Warning("Invalid masked object: %s", maskedId.AsChar());
                }
            } else {
                Log::Warning("invalid mask object: %s", maskId.AsChar());
            }
        } while (ps_block_next(ss));
        ps_block_end(ss);
    }

    if (ps_var_begin(ss, "interactiveChildren")) {
        container->interactiveChildren = ps_var_f(ss) != 0.0f;
    }

    if (ps_block_begin(ss, "masked")) {
        do {
            std::vector<int> path;
            if (ps_var_begin(ss, "path") && ParseVarPath(ss, path)) {
                if (const auto child = container->GetChildByPath(path)) {
                    std::vector<int> maskPath;
                    ps_var_begin(ss, "mask");
                    do {
                        if (ParseVarPath(ss, maskPath)) {
                            if (const auto mask = dynamic_cast<nsVisualMask*>(container->GetChildByPath(maskPath))) {
                                child->masks.push_back(mask);
                            }
                        }
                    } while (ps_var_next(ss));
                }
            }
        } while (ps_block_next(ss));
    }

    return true;
}

bool nsVisualContainerBuilder::SerializeProps(nsScriptSaver &saver, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!nsVisualBuilder2d::SerializeProps(saver, o, context)) {
        return false;
    }

    const auto container = Cast<nsVisualContainer2d>(o);
    if (!container) {
        return false;
    }

    auto &children = container->GetChildren();

    saver.VarBool("interactiveChildren", container->interactiveChildren, true);

    if (children.empty()) {
        return true;
    }

    if (saver.BlockBegin("children")) {
        bool res = true;
        for (const auto child : children) {
            if (const auto builder = context->GetBuilder(child->GetType())) {
                res &= builder->Serialize(saver, child, context);
            } else {
                res = false;
            }
        }

        saver.BlockEnd();
        if (!res) {
            return false;
        }
    }

    if (!container->GetParent()) {
        container->IterateRecursive([&](nsVisualObject2d *child) {
            if (!child->masks.empty()) {
                if (saver.BlockBegin("masked")) {
                    std::vector<int> path;
                    if (container->GetChildPath(child, path)) {
                        VarPath(saver, "path", path);
                        for (const auto mask : child->masks) {
                            if (container->GetChildPath(mask, path)) {
                                VarPath(saver, "mask", path);
                            }
                        }
                    }
                    saver.BlockEnd();
                }
            }
            return true;
        });
    }

    return true;
}

void nsVisualContainerBuilder::VarPath(const nsScriptSaver &saver, const char *name, const std::vector<int> &path) {
    std::string s;

    for (size_t i = 0; i < path.size(); ++i) {
        s += std::to_string(path[i]);
        if (i != path.size() - 1) {
            s += ",";
        }
    }

    saver.VarString(name, s.c_str());
}

bool nsVisualContainerBuilder::ParseVarPath(script_state_t *ss, std::vector<int> &path) {
    path.clear();

    const auto str = ps_var_str(ss);
    if (!ss) {
        return false;
    }

    const std::string source(str);

    std::stringstream stream(source);
    std::string item;

    while (std::getline(stream, item, ',')) {
        try {
            path.push_back(std::stoi(item));
        } catch (...) {
            return false;
        }
    }

    return !path.empty();
}
