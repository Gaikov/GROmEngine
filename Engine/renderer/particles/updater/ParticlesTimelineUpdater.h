// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ParticlesTimelineUpdater.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "ParticlesUpdater.h"
#include "nsLib/log.h"
#include "Core/ParserUtils.h"

template <class TFrameData>
class nsParticlesTimelineUpdater : public nsParticlesUpdater {
public:
    void Update(nsParticle *p, float deltaTime) override {
        float timeLine = p->timeLine;

        auto prevFrame = timeline[0];
        for (int i = 0; i < timeline.size(); i++) {
            auto &f = timeline[i];
            if (timeLine < f->time) {
                float delta = f->time - prevFrame->time;
                float t = 0;
                if (delta > 0) {
                    t = (timeLine - prevFrame->time) / delta;
                }

                LerpFrame(p, prevFrame->data, f->data, t);
                return;
            }
            prevFrame = f;
        }


    }

    bool Parse(script_state_t *ss, nsParticlesUpdaterContext *context) override {
        if (ps_block_begin(ss, "frame")) {
            float time = 0;
            do {
                auto   frame = std::make_shared<Frame>();
                time += abs(ParseFloat(ss, "time"));
                frame->time = time;
                if (ParseFrame(ss, frame->data)) {
                    timeline.push_back(frame);
                }
            } while (ps_block_next(ss));
            ps_block_end(ss);

            for (auto &f : timeline) {
                f->time = std::clamp(f->time, 0.0f, 1.0f);
            }
            std::sort(timeline.begin(), timeline.end(), [](const auto &f1, const auto &f2) { return f1->time < f2->time; });

            Validate();
        } else {
            Log::Warning("Frame info not found!");
            return false;
        }

        return true;
    }

    void Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) override {
        float prevTime = 0;
        for (auto &f : timeline) {
            if (ss->BlockBegin("frame")) {
                ss->VarFloat("time", f->time - prevTime, -1);
                prevTime = f->time;
                SaveFrame(ss, f->data);
                ss->BlockEnd();
            }
        }
    }

protected:
    virtual bool ParseFrame(script_state_t *ss, TFrameData &frame) = 0;
    virtual void SaveFrame(nsScriptSaver *ss, TFrameData &frame) = 0;
    virtual void LerpFrame(nsParticle *p, const TFrameData &f1, const TFrameData &f2, float t) = 0;
    virtual void Validate() = 0;

public:
    struct Frame {
        float       time;
        TFrameData  data;
    };

    std::vector<std::shared_ptr<Frame>>  timeline;
};