// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file engine.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "engine.h"

#include "display/particles/effects/ParticlesEffectHolder.h"
#include "nsLib/matrix4.h"
#include "Core/Core.h"
#include "Core/Config.h"
#include "nsLib/log.h"
#include "GameApp.h"
#include "RenManager.h"
#include "RenAux.h"
#include "Console.h"
#include "Input.h"
#include "SndManager.h"
#include "Anim.h"
#include "Platform.h"
#include "input/SoftKeyboard.h"
#include "display/VisualSceneRender2d.h"
#include "EngineContext.h"
#include "debug/DebugDrawManager.h"
#include "debug/policies/FastMemDebugDraw.h"
#include "renderer/particles/ParticlesManager.h"
#include "renderer/font/FontsCache.h"
#include "display/factory/VisualFactory2d.h"
#include "display/pool/LayoutsPool.h"
#include "display/pool/VisualsPoolDebugDraw.h"

#define DEBUG_BUILD "Debug"

dword	prevTime;
uint	frametime;
float	g_frameTime;

nsVar	*com_maxfps = nullptr;
nsVar	*com_min_time = nullptr;
nsVar	*com_time_scale = nullptr;

nsVar	*r_drawfps = nullptr;

extern void pause_f( int argc, const char *argv[] );

//---------------------------------------------------------
// nsEngine::Init: 
//---------------------------------------------------------
bool nsEngine::Init()
{
	IGameApp	*app = App_GetGame();

	g_frameTime = 0;
	if ( !nsCore::Init() ) return false;

    Log::Info("...Starting engine");
	com_maxfps = g_cfg->RegVar( "com_maxfps", "0", GVF_SAVABLE );
	com_min_time = g_cfg->RegVar( "com_min_time", "200", 0 );
	com_time_scale = g_cfg->RegVar( "com_time_scale", "0", 0 );
	r_drawfps = g_cfg->RegVar( "r_drawfps", "0", GVF_SAVABLE );

	g_cfg->RegCmd( "pause", pause_f );
	g_cfg->RegCmd( "anim_make", nsAnim::anim_make_f );

	nsRenDevice::RegVars();

	if ( !app->InitDialog() ) return false;

    if (!App_GetPlatform()->Init()) return false;

    nsSoundDevice::Init();
    if (!nsRenDevice::Init()) return false;
    if (!nsFontsCache::Init()) return false;

	g_inp.ShowCursor( false );
	nsConsole::Init();

    if (!nsEngineContext::Init()) {
        return false;
    }

    if (!nsParticlesManager::Init()) {
        return false;
    }

    nsVisualFactory2d::Init();
	const auto vf = nsVisualFactory2d::Shared();
	vf->BindClass<nsParticlesEffect>("ParticlesEffect");
	vf->BindClass<nsParticlesEffectHolder>("ParticlesEffectHolder");

	nsLayoutsPool::Init();
	nsDebugDrawManager::Init();
	auto ddm = nsDebugDrawManager::Shared();
	ddm->AddPolicy(new nsFastMemDebugDraw());
	ddm->AddPolicy(new nsVisualsPoolDebugDraw());

	if ( !app->Init() ) return false;

	prevTime = App_GetPlatform()->GetTime();

	g_inp.Init();

    g_cfg->RegCmd("ad_test", [](int, const char* []) {
        auto res = App_GetPlatform()->ShowInterstitialAd();
        Log::Info(res ? "shown" : "failed");
    });

    g_cfg->RegCmd("si_show", [](int argc, const char *argv[]) -> void {
        App_GetPlatform()->GetSoftInput()->Show();
    });

    g_cfg->RegCmd("si_hide", [](int argc, const char *argv[]) -> void {
        App_GetPlatform()->GetSoftInput()->Hide();
    });

	return true;
}

//---------------------------------------------------------
// nsEngine::Release: 
//---------------------------------------------------------
void nsEngine::Release(bool failed)
{

	LogPrintf( PRN_ALL,
				"----------------------------------------\n"\
				"********** Shutting Down App ***********\n"\
				"----------------------------------------\n" );

    if ( !failed )
        g_cfg->SaveConfig();

	g_inp.Release();
	App_GetGame()->Release();

	nsDebugDrawManager::Release();
	nsLayoutsPool::Release();
    nsVisualFactory2d::Release();
    nsParticlesManager::Release();
    nsEngineContext::Release();
	nsConsole::Release();
    nsFontsCache::Release();
	nsRenDevice::Release();
	nsSoundDevice::Release();

    App_GetPlatform()->Release();
	nsCore::Release();
}

//---------------------------------------------------------
// nsEngine::MainLoop: 
//---------------------------------------------------------
#define		MIN_FPS	60
#define		MIN_SEC (1.0f / MIN_FPS)
#define		MIN_MS (MIN_SEC * 1000.0f)

void GetElapsedTime( uint &res, dword &currtime )
{
	do
	{
		currtime = App_GetPlatform()->GetTime();
		res = currtime - prevTime;
	}
	while ( res < 1 );
}

dword	add = 0;

void nsEngine::MainLoop()
{
	nsMemory::BeginLoop();
	IGameApp	*game = App_GetGame();

	dword	currtime = 0;
	GetElapsedTime( frametime, currtime );

	//correct frame time
	if ( com_min_time->Value() > 0 )
	{
		if ( frametime > (uint)com_min_time->Value() )
			frametime = (uint)com_min_time->Value();
	}

	if ( com_maxfps->Value() > 0 ) {
		if ( frametime < 1000 / com_maxfps->Value() ) {
			nsMemory::EndLoop();
			return;
		}
	}


	{
		auto count = int(frametime / MIN_MS);
		if ( !count )
		{
			g_frameTime = frametime / 1000.0f;
			if ( com_time_scale->Value() > 0 ) g_frameTime *= com_time_scale->Value();

			g_inp.Process();
			game->Loop( g_frameTime );
			nsSoundDevice::Shared()->Device()->Update();
		}
		else
		{
			if ( count > MIN_FPS )
				count = MIN_FPS;
			else
			{
				add = frametime - uint(count * MIN_MS);
				currtime -= add;
			}

			for ( int i = 0; i < count; ++i )
			{
				g_frameTime = MIN_SEC;
				if ( com_time_scale->Value() > 0 ) g_frameTime *= com_time_scale->Value();

				g_inp.Process();
				game->Loop( g_frameTime );
                nsSoundDevice::Shared()->Device()->Update();
			}
		}
	}

	prevTime = currtime;

	if ( g_renDev->BeginScene() )
	{
		game->DrawWorld();

        auto context = nsEngineContext::Shared();
        auto &ortho = context->GetUIOrtho();
        auto root = context->GetUIRoot();

        int w, h;
        App_GetPlatform()->GetClientSize(w, h);
        auto screenWidth = (float)w;
        auto screenHeight = (float)h;
        ortho.SetScreenSize(screenWidth, screenHeight);
        g_renDev->LoadProjMatrix( ortho.GetViewMatrix() );

		g_renDev->LoadMatrix( nsMatrix::identity );
		g_renDev->LoadViewMartix( nsMatrix::identity );
		g_renDev->StateApply( context->GetUIState() );

		if ( r_drawfps->Value() > 0 ) RX_DrawFPS( 10, 10, frametime );

        nsSoftKeyboard::Shared()->UpdateScale(screenWidth, screenHeight);
        root->SetContentWidth(screenWidth);
        root->SetContentHeight(screenHeight);
        nsVisualSceneRender2d::DrawScene(root);

        g_renDev->LoadMatrix( nsMatrix::identity );
		nsDebugDrawManager::Shared()->Draw();
		nsConsole::Shared()->Draw();

#ifdef DEBUG
        auto sysFont = nsFontsCache::Shared()->SysFont();
        float size[2];
        sysFont->GetSize(DEBUG_BUILD, size, 0);
        sysFont->Draw(DEBUG_BUILD, nsVec2(w - size[0] / 2 - 10, 10), nsVec2(0.5, 1), nsColor::white, 0);
#endif

        g_renDev->EndScene();
        App_GetPlatform()->SwapBuffers();
	}

	nsMemory::EndLoop();
}

void nsEngine::OnKeyDown(int keyCode, bool repeat)
{
	//Log::Debug("key down: %i", keyCode);
    auto console = nsConsole::Shared();
    if (keyCode == NS_KEY_TILDE && console->IsEnabled()) {
        console->Toggle();
    } else {
        auto ui = GetActiveInput();
        if (ui) {
            ui->OnKeyDown(keyCode, repeat);
        }
    }
}

IUserInput *nsEngine::GetActiveInput() {
    return nsEngineContext::Shared()->GetActiveInput();
}

void nsEngine::OnPointerMove(int pointerId, int clientX, int clientY) {
    auto ui = GetActiveInput();
    if (ui) {
        ui->OnPointerMove((float)clientX, (float)clientY, pointerId);
    }
}

void nsEngine::OnPointerDown(int pointerId, int clientX, int clientY) {
    auto ui = GetActiveInput();
    if (ui) {
        ui->OnPointerDown((float)clientX, (float)clientY, pointerId);
    }
}

void nsEngine::OnPointerUp(int pointerId, int clientX, int clientY) {
    auto ui = GetActiveInput();
    if (ui) {
        ui->OnPointerUp((float)clientX, (float)clientY, pointerId);
    }
}

void nsEngine::OnKeyUp(int keyCode) {
    //Log::Debug("on key up: %i", keyCode);
    auto ui = GetActiveInput();
    if (ui) ui->OnKeyUp(keyCode);
}

void nsEngine::OnCharDown(char ch) {
    auto ui = GetActiveInput();
    if (ui) ui->OnChar(ch);
}

void nsEngine::OnActivateApp(bool active) {
    LogPrintf(PRN_ALL, "activate :%i\n", active ? 1 : 0);

    //SDL_CaptureMouse(SDL_FALSE);

    prevTime = App_GetPlatform()->GetTime();
    g_inp.OnActivateApp(active);
    nsSoundDevice::Shared()->Device()->OnActivate(active);
    App_GetGame()->OnActivate(active);

    if (!active) {
        //Unload assets from hardware on mobile devices
#ifdef MOBILE
        g_renDev->InvalidateResources();
#endif
        g_cfg->SaveConfig();
    }
}

