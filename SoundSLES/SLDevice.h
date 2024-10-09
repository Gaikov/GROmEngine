//
// Created by Roman on 4/2/2024.
//

#pragma once
#include "SLCommon.h"

class SLDevice : public ISndDevice {
public:
    bool Init() override;

    void Release() override;

    void OnActivate(bool active) override;

    void SetListener(const nsVec3 &pos, const nsVec3 &at, const nsVec3 &up) override;

    void SetGameVolume(float sfxVolume, float musVolue) override;

    void Update() override;

    ISound *SoundLoad(const char *fileName) override;

    void SoundFree(ISound *snd) override;

    IVoice *SoundPlay(ISound *snd, bool loop) override;

    IVoice *SoundPlay3D(ISound *snd, const nsVec3 &pos, bool loop, float speedScale) override;

    bool VoiceIsPlaying(IVoice *v, ISound *snd) override;

    void VoiceStop(IVoice *v) override;

    void VoiceRolloff(IVoice *v, float rollOff) override;

    void VoiceSetPos(IVoice *v, const nsVec3 &pos) override;

    void VoiceSetVolume(IVoice *v, float volume) override;

    void VoiceStopAll(bool onlyLooped) override;

    void VoicePauseAll(bool pause, bool onlyLooped) override;

    ITrack *TrackLoad(const char *fileName) override;

    void TrackFree(ITrack *t) override;

    void TrackPlay(ITrack *t) override;

    bool TrackIsPlaying(ITrack *t) override;

    void TrackPause(ITrack *t) override;

    void TrackStop(ITrack *t) override;

    uint TrackGetPos(ITrack *t) override;

    void TrackSetPos(ITrack *t, uint pos) override;

    void TrackSetVolume(ITrack *t, float volume) override;
};