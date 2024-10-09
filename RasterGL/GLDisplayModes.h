//
// Created by Roman Gaikov on 2019-09-20.
//

#ifndef __GLDISPLAYMODES_H__
#define __GLDISPLAYMODES_H__

#include "GLCommon.h"

class GLDisplayModes
{
public:
	GLDisplayModes();
	bool Init();
	bool IsValid(int width, int height);
	bool ApplyCurrentMode();
	const rasterConfig_t &GetConfig();

	const Platform::tDisplayModesList& GetModes();

private:
    Platform::tDisplayModesList _modes;
	rasterConfig_t              _cfg;

private:
    const Platform::DisplayMode* FindValidMode(int width, int height);
    const Platform::DisplayMode* ValidateCurrentDisplayMode();
	bool ApplyFullScreenMode();
	void ApplyWindowMode();

};


#endif //__GLDISPLAYMODES_H__
