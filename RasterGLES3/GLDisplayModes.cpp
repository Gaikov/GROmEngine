//
// Created by Roman Gaikov on 2019-09-20.
//

#include "GLDisplayModes.h"
#include "Core/Config.h"
#include "GLUtils.h"

GLDisplayModes::GLDisplayModes() : _cfg{32, 800, 600}
{

}

bool GLDisplayModes::Init()
{
	return true;
}

bool GLDisplayModes::IsValid(int width, int height)
{
	return FindValidMode(width, height);
}

const Platform::DisplayMode* GLDisplayModes::FindValidMode(int width, int height)
{
    GetModes();
	for (auto &mode : _modes)
	{
		if (mode.width == width && mode.height == height)
		{
			return &mode;
		}
	}
	return nullptr;
}

const rasterConfig_t &GLDisplayModes::GetConfig()
{
	return _cfg;
}

bool GLDisplayModes::ApplyCurrentMode()
{
	if (r_fullscreen->Bool())
	{
		if (!ApplyFullScreenMode())
		{
			r_fullscreen->SetValue(0);
			ApplyWindowMode();
		}
	}
	else
	{
		ApplyWindowMode();
	}

	glViewport(0, 0, (int)_cfg.currWidth, (int)_cfg.currHeight);
	GL_CHECK_R("glViewport", false)

	return true;
}

bool GLDisplayModes::ApplyFullScreenMode()
{
    auto mode = ValidateCurrentDisplayMode();
    if (!mode) {
        return false;
    }
	LogPrintf(PRN_ALL, "Applying full screen mode: %ix%i...\n", _cfg.currWidth, _cfg.currHeight);
    return App_GetPlatform()->ApplyDisplayMode((int)_cfg.currWidth, (int)_cfg.currHeight, true, mode->bits, mode->frequency);
}

void GLDisplayModes::ApplyWindowMode()
{
    _cfg.currWidth = (int)r_width->Value();
    _cfg.currHeight = (int)r_height->Value();

	LogPrintf(PRN_ALL, "Applying window mode: %ix%i\n", _cfg.currWidth, _cfg.currHeight);
    App_GetPlatform()->ApplyDisplayMode((int)_cfg.currWidth, (int)_cfg.currHeight, false, 0, 0);
}

const Platform::tDisplayModesList& GLDisplayModes::GetModes()
{
    _modes.clear();
    App_GetPlatform()->GetDisplayModes(_modes);
    return _modes;
}

const Platform::DisplayMode *GLDisplayModes::ValidateCurrentDisplayMode() {
    int w = (int) r_width->Value();
    int h = (int) r_height->Value();

    auto mode = FindValidMode(w, h);
    if (mode) {
        _cfg.currWidth = w;
        _cfg.currHeight = h;
        return mode;
    }
    else
    {
        Log::Warning("Invalid display mode: %i x %i", w, h);

        auto &modes = GetModes();
        if (!modes.empty()) {
            mode = &modes[0];
            r_width->SetValue(_cfg.currWidth = mode->width);
            r_height->SetValue(_cfg.currHeight = mode->height);
            Log::Info("Selected a valid display mode: %i x %i", mode->width, mode->height);
            return mode;
        }
    }

    Log::Warning("Could not find a valid display mode!");
    return nullptr;
}



