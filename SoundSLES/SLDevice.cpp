//
// Created by Roman on 4/2/2024.
//

#include "SLDevice.h"

static SLDevice g_slDevice;

ISndDevice*	GetSoundDevice()
{
    return &g_slDevice;
}

bool SLDevice::Init() {
    return true;
}

void SLDevice::Release() {

}

void SLDevice::OnActivate(bool active) {

}

void SLDevice::SetListener(const nsVec3 &pos, const nsVec3 &at, const nsVec3 &up) {

}

void SLDevice::SetGameVolume(float sfxVolume, float musVolue) {

}

void SLDevice::Update() {

}

ISound *SLDevice::SoundLoad(const char *fileName) {
    return nullptr;
}

void SLDevice::SoundFree(ISound *snd) {

}

IVoice *SLDevice::SoundPlay(ISound *snd, bool loop) {
    return nullptr;
}

IVoice *SLDevice::SoundPlay3D(ISound *snd, const nsVec3 &pos, bool loop, float speedScale) {
    return nullptr;
}

bool SLDevice::VoiceIsPlaying(IVoice *v, ISound *snd) {
    return false;
}

void SLDevice::VoiceStop(IVoice *v) {

}

void SLDevice::VoiceRolloff(IVoice *v, float rollOff) {

}

void SLDevice::VoiceSetPos(IVoice *v, const nsVec3 &pos) {

}

void SLDevice::VoiceSetVolume(IVoice *v, float volume) {

}

void SLDevice::VoiceStopAll(bool onlyLooped) {

}

void SLDevice::VoicePauseAll(bool pause, bool onlyLooped) {

}

ITrack *SLDevice::TrackLoad(const char *fileName) {
    return nullptr;
}

void SLDevice::TrackFree(ITrack *t) {

}

void SLDevice::TrackPlay(ITrack *t) {

}

bool SLDevice::TrackIsPlaying(ITrack *t) {
    return false;
}

void SLDevice::TrackPause(ITrack *t) {

}

void SLDevice::TrackStop(ITrack *t) {

}

uint SLDevice::TrackGetPos(ITrack *t) {
    return 0;
}

void SLDevice::TrackSetPos(ITrack *t, uint pos) {

}

void SLDevice::TrackSetVolume(ITrack *t, float volume) {

}
