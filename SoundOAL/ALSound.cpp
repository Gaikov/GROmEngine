// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ALSound.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "ALSound.h"
#include "OALUtils.h"
#include "nsLib/log.h"
#include "Core/Memory.h"
#include "SoundBase/OggReader.h"
#include "SoundBase/WavReader.h"

//---------------------------------------------------------
// nsALSound::nsALSound: 
//---------------------------------------------------------
nsALSound::nsALSound(const char *fileName) : prev(nullptr),
                                             next(nullptr),
                                             m_fileName(fileName),
                                             m_refCount(0),
                                             m_alBuff(0) {
}

//---------------------------------------------------------
// nsALSound::~nsALSound:
//---------------------------------------------------------
nsALSound::~nsALSound() {
    Free();
}

//---------------------------------------------------------
// nsALSound::Reload:
//---------------------------------------------------------
bool nsALSound::Reload() {
    if (!m_fileName.Length()) return false;

    if (!Free()) {
        LogPrintf(PRN_ALL, "WARNING: al buffer is busy!\n");
        return false;
    }

    if (strstr(m_fileName, ".ogg")) {
        if (!CreateSBFromOgg(m_fileName, true))
            return false;
    } else if (!CreateSBFromWav(m_fileName, true))
        return false;

    return true;
}

//---------------------------------------------------------
// nsALSound::Free:
//---------------------------------------------------------
bool nsALSound::Free() {
    if (!m_alBuff) return true;

    alDeleteBuffers(1, &m_alBuff);
    if (const ALenum code = alGetError()) {
        LogPrintf(PRN_ALL, "WARNING: delete buffer: %s\n", AL_GetError(code));
        return false;
    }

    m_alBuff = 0;
    return true;
}

//---------------------------------------------------------
// nsALSound::CreateSBFromOgg:
//---------------------------------------------------------
bool nsALSound::CreateSBFromOgg(const char *fileName, bool sound3d) {
    nsOggReader ogg;
    ALenum code;

    alGenBuffers(1, &m_alBuff);
    if ((code = alGetError())) {
        Log::Error("gen buffer: %s", AL_GetError(code));
        return false;
    }

    if (!ogg.StartReading(fileName, false)) {
        return false;
    }

    long size;
    const auto buff = ogg.ReadWhole(size);
    if (!buff) {
        return false;
    }

    ALenum format = 0;
    const vorbis_info *info = ogg.GetVorbisInfo();
    if (info->channels > 1)
        format = AL_FORMAT_STEREO16;
    else
        format = AL_FORMAT_MONO16;

    alBufferData(m_alBuff, format, buff, size, info->rate);
    my_free(buff);

    if ((code = alGetError())) {
        Log::Error("copy OGG to buff: %s\n", AL_GetError(code));
        return false;
    }

    return true;
}

//---------------------------------------------------------
// nsALSound::CreateSBFromWav:
//---------------------------------------------------------
bool nsALSound::CreateSBFromWav(const char *filename, bool sound3d) {
    nsWavReader wav;
    ALenum code;

    if (!wav.Read(filename)) {
        return false;
    }

    auto fmt = wav.GetFormat();
    if (sound3d && fmt->nChannels > 1) {
        Log::Warning("invalid 3D sound $s!", filename);
        return false;
    }

    alGenBuffers(1, &m_alBuff);
    if ((code = alGetError())) {
        Log::Error("gen buffer: %s\n", AL_GetError(code));
        return false;
    }

    ALenum format = 0;
    if (fmt->nChannels > 1)
        format = fmt->wBitsPerSample == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    else
        format = fmt->wBitsPerSample == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;

    alBufferData(m_alBuff, format, wav.GetSamples(), wav.GetSamplesSize(), fmt->nSamplesPerSec);
    if ((code = alGetError())) {
        LogPrintf(PRN_DEV, "\nch: %i\nbits: %i\nhz: %i\n",
                  fmt->nChannels,
                  fmt->wBitsPerSample,
                  fmt->nSamplesPerSec);
        Log::Error("ERROR: copy WAV to buff: %s\n", AL_GetError(code));
        return false;
    }

    return true;
}
