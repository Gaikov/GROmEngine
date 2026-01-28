//
// Created by Roman on 10/14/2024.
//
#include "VisualBuilder2d.h"
#include "Core/ParserUtils.h"
#include "display/property/VisualCustomPropFactory.h"

nsVisualObject2d *nsVisualBuilder2d::Create(script_state_t *ss, nsVisualCreationContext2d *context) {
    auto visual = context->CreateByID(ParseString(ss, "bindingId"));
    if (!visual) {
        visual = CreateDefault();
    }
    return visual;
}

bool nsVisualBuilder2d::Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (const auto id = ParseStrP(ss, "id")) {
        o->id = id;
    }
    if (const auto bindingId = ParseStrP(ss, "bindingId")) {
        o->bindingId = bindingId;
    }

    ParseAnchor(ss, "xMin", o->xMin);
    ParseAnchor(ss, "yMin", o->yMin);
    ParseAnchor(ss, "xMax", o->xMax);
    ParseAnchor(ss, "yMax", o->yMax);
    ParseAnchor(ss, "xCenter", o->xCenter);
    ParseAnchor(ss, "yCenter", o->yCenter);

    o->origin.pos = {
            ParseFloat(ss, "x", o->origin.pos->x),
            ParseFloat(ss, "y", o->origin.pos->y)};

    o->origin.scale = {
            ParseFloat(ss, "sx", o->origin.scale->x),
            ParseFloat(ss, "sy", o->origin.scale->y)};

    if (ps_var_begin(ss, "angle")) {
        o->origin.angle = nsMath::ToRad(ps_var_f(ss));
    }

    o->visible = ParseBool(ss, "visible", o->visible);

    if (ps_block_begin(ss, "custom_props")) {
        if (ps_block_begin(ss, nullptr)) {
            do {
                std::string name = ps_block_name(ss);
                if (const auto prop = context->propsFactory.Deserialize(ss)) {
                    o->customProps[name] = prop;
                }
            } while (ps_block_next(ss));
        }
        ps_block_end(ss);
    }

    return true;
}

bool nsVisualBuilder2d::SerializeProps(nsScriptSaver &saver, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    if (!o->id.empty()) {
        saver.VarString("id", o->id.c_str());
    }
    if (!o->bindingId.empty()) {
        saver.VarString("bindingId", o->bindingId.c_str());
    }

    SaveAnchor(saver, "xMin", o->xMin);
    SaveAnchor(saver, "yMin", o->yMin);
    SaveAnchor(saver, "xMax", o->xMax);
    SaveAnchor(saver, "yMax", o->yMax);
    SaveAnchor(saver, "xCenter", o->xCenter);
    SaveAnchor(saver, "yCenter", o->yCenter);

    saver.VarFloat("x", o->origin.pos->x, 0);
    saver.VarFloat("y", o->origin.pos->y, 0);
    saver.VarFloat("sx", o->origin.scale->x, 1);
    saver.VarFloat("sy", o->origin.scale->y, 1);
    saver.VarFloat("angle",  nsMath::ToDeg(o->origin.angle), 0);
    saver.VarBool("visible", o->visible, true);

    if (!o->customProps.empty()) {
        if (saver.BlockBegin("custom_props")) {
            for (const auto &prop : o->customProps) {
                if (saver.BlockBegin(prop.first.c_str())) {
                    context->propsFactory.Serialize(saver, prop.second);
                    saver.BlockEnd();
                }
            }

            saver.BlockEnd();
        }
    }

    return true;
}

bool nsVisualBuilder2d::Serialize(nsScriptSaver &saver, nsVisualObject2d *o, nsVisualCreationContext2d *context) {
    bool res = false;
    if (saver.BlockBegin(o->GetType())) {
        res = SerializeProps(saver, o, context);
        saver.BlockEnd();
    }
    return res;
}

bool nsVisualBuilder2d::ParseAnchor(script_state_t *ss, const char *name, nsLayoutAnchor &anchor) {
    float val[2];
    if (ParseFloat2(ss, name, val)) {
        anchor.value = val[0];
        anchor.type = val[1] > 0 ? nsLayoutAnchor::PERCENT : nsLayoutAnchor::NUMBER;
        return true;
    }
    return false;
}

void nsVisualBuilder2d::SaveAnchor(nsScriptSaver &saver, const char *name, const nsLayoutAnchor &anchor) {
    if (anchor.IsUsed()) {
        saver.Printf("$%s %f %i", name, static_cast<float>(anchor.value), anchor.type == nsLayoutAnchor::PERCENT ? 1 : 0);
    }
}



