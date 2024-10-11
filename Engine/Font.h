// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Font.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/StrTools.h"
#include "Engine/RenDevice.h"
#include "nsLib/math/Rect.h"
#include "nsLib/FilePath.h"

#define		RES_FONT	0x5555

typedef struct
{
	ITexture	*tex;
	rchar_t		r;
    float       xadvance;
}
fchar_t;

typedef struct
{
	float	width, height;
	float	offs_x, offs_y;
}
char_desc_t;

#define	MAX_CHARS	256

//---------------------------------------------------------
// nsFont: 
//---------------------------------------------------------
struct nsFont final
{
public:
	nsString	m_fileName;
	std::vector<ITexture*>  _pages;

	float		avg_height = 0;
	
	fchar_t		ch[MAX_CHARS] = {};

public:
    nsFont();
    bool		Load( const char *filename );
	void		Free();
	void		Draw( const char *str, float pos[2], float scale[2], const float c[4], int len = 0, float fixedWidth = 0 );
	void		DrawFX( const char *str, float pos[2], float scale[2], const float c[4], float scale2[2], const float c2[2], int len );
	void		DrawAlphaFX( const char *str, float pos[2], float scale[2], const float c1[4], float lerp );
	void		GetSize( const char *str, float size[2], int len = 0 );
    void        GetBounds( const char *str, nsRect &bounds);
	rchar_t*	GetRChar( uchar ch );
	void		GetCharDesc( uchar ch, char_desc_t &cd );

private:
    IRenDevice  *_device;

    bool LoadGROmFont(const nsFilePath &filePath);
    bool LoadBitmapFont(const nsFilePath &filePath);
};

