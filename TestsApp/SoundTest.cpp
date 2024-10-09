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
		LogPrintf(PRN_ALL, "Not enough params");
		return;
	}

	auto fileName = argc[1];
	auto device = GetSoundDevice();
	auto sound = device->SoundLoad(fileName);
	if (!sound) {
		LogPrintf(PRN_ALL, "sound not found");
		return;
	}

	LogPrintf(PRN_ALL, "Playing: %s\n", fileName);
	device->SoundPlay(sound);
}
