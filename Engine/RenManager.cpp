#include "RenManager.h"
#include "Core/Config.h"
#include "Core/Memory.h"
#include "Font.h"
#include "RenAux.h"
#include "Platform.h"
#include "nsLib/log.h"

/**
 * @deprecated
 */
IRenDevice		*g_renDev = nullptr;
nsFont			*g_sysFont = nullptr;

nsVar	*r_fullscreen = nullptr;
nsVar	*r_pixel_bits = nullptr;
nsVar	*r_tex_bits = nullptr;
nsVar	*r_vsync = nullptr;
nsVar	*r_tex_filter = nullptr;
nsVar	*r_brightness = nullptr;
nsVar	*r_width = nullptr;
nsVar	*r_height = nullptr;
nsVar	*r_refresh = nullptr;

extern IRenDevice* GetRenDevice();

//---------------------------------------------------------
// nsRenDevice::RegVars:
//---------------------------------------------------------
bool nsRenDevice::RegVars()
{
    g_cfg->RegCmd("r_invalidate", Invalidate_f);
    g_cfg->RegCmd("r_portrait", Portrait_f);

	r_fullscreen = g_cfg->RegVar( "r_fullscreen", "0", GVF_SAVABLE );
	r_pixel_bits = g_cfg->RegVar( "r_pixel_bits", "16", GVF_SAVABLE );
	r_tex_bits = g_cfg->RegVar( "r_tex_bits", "16", GVF_SAVABLE );
	r_vsync = g_cfg->RegVar( "r_vsync", "1", GVF_SAVABLE );
	r_tex_filter = g_cfg->RegVar( "r_tex_filter", "1", GVF_SAVABLE );
	r_brightness = g_cfg->RegVar( "r_brightness", "0.5", GVF_SAVABLE );

	r_width = g_cfg->RegVar( "r_width", "640", GVF_SAVABLE );
	r_height = g_cfg->RegVar( "r_height", "480", GVF_SAVABLE );
	r_refresh = g_cfg->RegVar( "r_refresh", "0", GVF_SAVABLE );

	return true;
}

//---------------------------------------------------------
// nsRenDevice::Init:
//---------------------------------------------------------
bool nsRenDevice::OnInit() {
    nsSubSystem::OnInit();

    g_renDev = _current = GetRenDevice();

    if (!_current->Init(App_GetPlatform()->GetWindowHandler())) return false;

    g_sysFont = _defaultFont = new nsFont();
    _defaultFont->Load("default/sysfont.txt");

    return RX_Init();
}

//---------------------------------------------------------
// nsRenDevice::Release:
//---------------------------------------------------------
void nsRenDevice::OnRelease() {
    nsSubSystem::OnRelease();

	RX_Release();

	if ( _defaultFont )
	{
		_defaultFont->Free();
		delete _defaultFont;
		g_sysFont = _defaultFont = nullptr;
	}

	if ( _current ) {
        _current->Release();
        _current = nullptr;
        g_renDev = nullptr;
    }
}

void nsRenDevice::Invalidate_f(int, const char*[]) {
    nsRenDevice::Shared()->Device()->InvalidateResources();
}

void nsRenDevice::Portrait_f(int argc, const char **argv) {
    if (argc < 3) {
        Log::Info("usage: [height] [aspect]");
        return;
    }

    auto height = (float)atoi(argv[1]);
    if (height < 50) {
        Log::Warning("Invalid height!");
        return;
    }

    auto aspect = (float)atof(argv[2]);
    if (aspect < 0.3) {
        Log::Warning("Invalid aspect!");
        return;
    }

    r_height->SetValue(height);
    r_width->SetValue(height / aspect);
    g_cfg->ExecLine("r_restart");
}

