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
        Frame *frames = &_timeline[0];
        Frame *prevFrame = frames;
        for (int i = 0; i < _timeline.size(); i++) {
            auto f = &frames[i];
            timeLine -= frames[i].time;
            if (timeLine < 0) {
                auto t = (timeLine + f->time) / f->time;
                LerpFrame(p, prevFrame->data, f->data, t);
                break;
            }
            prevFrame = f;
        }
    }

    bool Parse(script_state_t *ss, nsParticlesUpdaterContext *context) override {
        if (ps_block_begin(ss, "frame")) {
            do {
                Frame   frame;
                frame.time = abs(ParseFloat(ss, "time"));
                if (ParseFrame(ss, frame.data)) {
                    _timeline.push_back(frame);
                }
            } while (ps_block_next(ss));
            ps_block_end(ss);

            float total = 0;
            for (auto &f : _timeline) {
                total += f.time;
            }

            if (total > 1) {
                Log::Warning("Timeline total time more than 1! Normalizing...");
                for (auto &f : _timeline) {
                    f.time /= total;
                }
            }

        } else {
            Log::Warning("Frame info not found!");
            return false;
        }

        return true;
    }

    void Save(nsScriptSaver *ss, nsParticlesUpdaterContext *context) override {
        for (auto &f : _timeline) {
            if (ss->BlockBegin("frame")) {
                ss->VarFloat("time", f.time, -1);
                SaveFrame(ss, f.data);
                ss->BlockEnd();
            }
        }
    }

protected:
    virtual bool ParseFrame(script_state_t *ss, TFrameData &frame) = 0;
    virtual void SaveFrame(nsScriptSaver *ss, TFrameData &frame) = 0;
    virtual void LerpFrame(nsParticle *p, const TFrameData &f1, const TFrameData &f2, float t) = 0;


private:
    struct Frame {
        float       time;
        TFrameData  data;
    };

    std::vector<Frame>  _timeline;
};