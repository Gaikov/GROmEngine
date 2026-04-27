//
// Created by Roman on 07/07/2019.
//

#include <nsLib/log.h>
#include <SoundBase/SndDevice.h>
#include "SoundTest.h"

void play_sound_f(int argv, const char **argc)
{
	if (argv < 2)
	{
		Log::Info("Not enough params");
		return;
	}

	auto fileName = argc[1];
	auto device = GetSoundDevice();
	auto sound = device->SoundLoad(fileName);
	if (!sound) {
		Log::Info("sound not found");
		return;
	}

	Log::Info("Playing: %s", fileName);
	device->SoundPlay(sound);
}
