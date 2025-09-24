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
#include "renderer/QuadsBuffer.h"

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
friend class nsFontsCache;

private:
	nsString	m_fileName;
	std::vector<ITexture*>  _pages;
	float		_lineHeight = 0;
	fchar_t		ch[MAX_CHARS] = {};

private:
    nsFont(nsQuadsBuffer *renBuffer);
	~nsFont();
    bool		Load( const char *filename );
	void		Free();

public:
    float       GetLineHeight() const { return _lineHeight; }
    ITexture*   GetPage(int index) { return _pages[index]; }
    void		Draw(const char *str, float pos[2], float scale[2], const float color[4], int len = 0, float fixedWidth = 0, float letterSpace = 0 );
	void		DrawFX( const char *str, float pos[2], float scale[2], const float c[4], float scale2[2], const float c2[2], int len );
	void		DrawAlphaFX( const char *str, float pos[2], float scale[2], const float c1[4], float lerp );
	void		GetSize( const char *str, float size[2], int len = 0 );
    void        GetBounds( const char *str, nsRect &bounds);
	rchar_t*	GetRChar( uchar ch );
	const fchar_t*	GetSymbolInfo( uchar code ) const;
	void		GetCharDesc( uchar ch, char_desc_t &cd );

private:
    IRenDevice  *_device;
	nsQuadsBuffer *_renBuffer;

	void DrawCharScaled(float x, float y, const rchar_t *rch, float sx, float sy, const nsColor &color) const;

    bool LoadGROmFont(const nsFilePath &filePath);
    bool LoadBitmapFont(const nsFilePath &filePath);
};

