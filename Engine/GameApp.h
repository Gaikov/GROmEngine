// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GameApp.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_GameApp_H_
#define	_GameApp_H_

#include "UserInput.h"

struct IGameApp
{
	virtual             ~IGameApp() = default;
	virtual bool		InitDialog() = 0;
	virtual bool		Init() = 0;	
	virtual void		Release() = 0;

	virtual void		DrawWorld() = 0;
	virtual void		Loop( float frameTime ) = 0;

	virtual void		OnActivate( bool active ) = 0;
	virtual void		OnPause( bool paused ) = 0;
	
	virtual int			GetWindowIcon() = 0;
	virtual IUserInput*	GetUserInput() = 0;
	
	virtual void		GetGUIDimension( int &width, int &height ) = 0;
	virtual const char* GetVersionInfo() = 0;
};

IGameApp*	App_GetGame();	//���������� ����������� � ������� ����

bool		App_IsPaused();
void		App_SetPause( bool pause );

#endif //_GameApp_H_