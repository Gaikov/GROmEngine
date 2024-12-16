// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Font.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "Font.h"
#include "nsLib/log.h"
#include "Core/Memory.h"
#include "Core/ParseFile.h"
#include "nsLib/color.h"
#include "nsLib/StrTools.h"
#include "RenManager.h"

nsFont::nsFont() {
    _device = nsRenDevice::Shared()->Device();
}

nsFont::~nsFont() {
    Free();
}

//---------------------------------------------------------
// nsFont::Load: 
//---------------------------------------------------------
bool nsFont::Load( const char *fileName )
{
    if ( !StrCheck( fileName ) ) return false;
    LogPrintf( PRN_ALL, "loading font: %s\n", fileName );


    nsFilePath  filePath(fileName);
    auto ext = filePath.GetExtension();
    ext.ToLower();

    m_fileName = filePath;

    if (ext == "txt")
        return LoadGROmFont(filePath);
    else if (ext == "fnt") {
        return LoadBitmapFont(filePath);
    }

    Log::Warning("Unsupported font format: %s", fileName);
    return false;
}

bool nsFont::LoadGROmFont(const nsFilePath &filePath) {
	nsParseFile		pf;
	script_state_t	*ss = pf.BeginFile( filePath );
	if ( !ss )
	{
		LogPrintf( PRN_ALL, "WARNING: can't parsing font!\n" );
		return false;
	}
	
	//-------------------------------------------
	if ( ps_block_begin( ss, "textures" ) 
		&& ps_var_begin( ss, "tex" ) )
	{
		do
		{
			ITexture *tex = g_renDev->TextureLoad( ps_var_str( ss ), false, TF_RGBA );
			if ( tex )
				_pages.push_back(tex);
		}
		while ( ps_var_next( ss ) );
		ps_block_end( ss );
	}
	
	if ( _pages.empty() )
	{
		LogPrintf( PRN_ALL, "WARNING: font textures not loaded!\n" );
		return false;
	}
	
	//-------------------------------------------
	int	i = 0;
    _lineHeight = 0;
	float	min_h = 0;
	float	max_h = 0;
	if ( ps_block_begin( ss, "chars_data" ) 
		&& ps_block_begin( ss, "char" ) )
	{
        bool	init_max = true;

        do
		{
			int	code;
			ps_var_begin( ss, "code" );
			code = (int)ps_var_f( ss );
			
			int	tex_id;
			ps_var_begin( ss, "tex_id" );
			tex_id = (int)ps_var_f( ss );
			ITexture *tex = tex_id >= 0 && tex_id < _pages.size() ? _pages[tex_id] : nullptr;

			if ( code >= 0 && code < MAX_CHARS && tex )
			{
				ch[code].tex = tex;
				
				ps_var_begin( ss, "coord" );
				ps_var_2f( ss, ch[code].r.coord );
				
				ps_var_begin( ss, "size" );
				ps_var_2f( ss, ch[code].r.size );
				
				ps_var_begin( ss, "offs" );
				ps_var_2f( ss, ch[code].r.offs );
				
				int	w = 1, h = 1;
				if ( ch[code].tex )
					ch[code].tex->GetSize( w, h );
				
				ch[code].r.tex_size[0] = ch[code].r.size[0] / float(w);
				ch[code].r.tex_size[1] = ch[code].r.size[1] / float(h);
				ch[code].r.coord[0] /= float(w);
				ch[code].r.coord[1] /= float(h);
                ch[code].xadvance = ch[code].r.offs[0] + ch[code].r.size[0];
				
				if ( init_max )
				{
					min_h = ch[code].r.offs[1];
					max_h = ch[code].r.offs[1] + ch[code].r.size[1];
					init_max = false;
				}
				else
				{
					if ( min_h > ch[code].r.offs[1] ) min_h = ch[code].r.offs[1];
					if ( max_h < ch[code].r.offs[1] + ch[code].r.size[1] )
						max_h = ch[code].r.offs[1] + ch[code].r.size[1];
				}
				
				i++;
			}
		}
		while ( ps_block_next( ss ) && i < MAX_CHARS );
	}
	
	if ( !i )
	{
		LogPrintf( PRN_ALL, "WARNING: no chars data present!\n" );
		return false;
	}
    _lineHeight = max_h - min_h;
	return true;
}

bool nsFont::LoadBitmapFont(const nsFilePath &filePath) {
    std::vector<int> charsParsed;
    auto file = g_pack.LoadFile(filePath);
    if (file) {
        const char *source = reinterpret_cast<const char *>(file->GetData());

        auto page = source;
        while ((page = strstr(page, "page "))) {
            int id;
            nsString    path;
            sscanf( page, "page id=%i file=%255s", &id, path.AsChar());
            if (!path.IsEmpty()) {
                auto texPath = filePath.GetParent().ResolvePath(path.Replace("\"", ""));
                _pages.push_back(_device->TextureLoad(texPath, false));
            }
            page ++;
        }

        auto symbol = source;
        while ((symbol = strstr(symbol, "char "))) {
            int charId = 0, pageId = 0;
            int x = 0, y = 0, width = 0, height = 0, xoffset = 0, yoffset = 0, xadvance = 0;

            sscanf(symbol, "char id=%i x=%i y=%i width=%i height=%i xoffset=%i yoffset=%i xadvance=%i page=%i",
            &charId, &x, &y, &width, &height, &xoffset, &yoffset, &xadvance, &pageId );

            if (charId >= 0 && charId < MAX_CHARS) {
                if (pageId >= 0 && pageId < _pages.size()) {
                    auto &charInfo = ch[charId];
                    charsParsed.push_back(charId);
                    auto tex = charInfo.tex = _pages[pageId];
                    charInfo.xadvance = (float)xadvance;

                    int texWidth, texHeight;
                    tex->GetSize(texWidth, texHeight);

                    auto &rc = charInfo.r;

                    rc.coord[0] = float(x) / float(texWidth);
                    rc.coord[1] = float(y) / float(texHeight);
                    rc.size[0] = float(width);
                    rc.size[1] = float(height);

                    rc.offs[0] = float(xoffset);
                    rc.offs[1] = -float(yoffset) - float(height);

                    rc.tex_size[0] = float(width) / float(texWidth);
                    rc.tex_size[1] = float(height) / float(texHeight);

                } else {
                    Log::Warning("Invalid page id: %i", pageId);
                }
            } else {
                Log::Warning("Invalid char id: %i", charId);
            }

            symbol ++;
        }


        if (!charsParsed.empty()) {
            _lineHeight = -1;
            auto common = strstr(source, "common");
            if (common) {
                int lineHeight = -1;
                sscanf(common, "common lineHeight=%i", &lineHeight);
                _lineHeight = (float)lineHeight;
            } else {
                Log::Warning("'common' header not found!");
            }

/*            if (_lineHeight > 0) {
                for (auto charId : charsParsed) {
                    auto &info = ch[charId];
                    info.r.offs[1] = info.r.offs[1] + _lineHeight;
                }
            } else {*/
            {
                float minOffset = ch[charsParsed[0]].r.offs[1];
                for (auto charId: charsParsed) {
                    auto &info = ch[charId];
                    minOffset = std::min(minOffset, info.r.offs[1]);
                }

                float height = 0;
                for (auto charId: charsParsed) {
                    auto &info = ch[charId];
                    info.r.offs[1] -= minOffset;

                    height = std::max(info.r.offs[1] + info.r.size[1], height);
                }

                if (_lineHeight <= 0) {
                    _lineHeight = height;
                }
            }
        } else {
            Log::Warning("No symbols parsed!");
        }
    }

    g_pack.ReleaseFile(file);
    return !charsParsed.empty() && !_pages.empty();
}

//------------------------------------
// Free: 
//------------------------------------
void nsFont::Free() {
    for (auto t: _pages) {
        g_renDev->TextureRelease(t);
    }
    memset(ch, 0, sizeof(ch));
}

//------------------------------------
// DrawContent:
//------------------------------------
void nsFont::Draw(const char *str, float pos[], float scale[], const float color[4], int len, float fixedWidth, float letterSpace)
{
	if ( !str || _pages.empty() ) return;
	
	g_renDev->SetColor(color );
	g_renDev->TextureBind( _pages[0] );
	auto	prev_tex = _pages[0];

	if ( !len ) len = (int)strlen( str );

	float	x = pos[0];
	float	s05 = scale[0] * 0.5f;
	while ( *str && len )
	{
		auto	chr = (unsigned char)*str;

        fchar_t	*c = &ch[chr];
		if ( c->tex )
		{
			if ( *str != 32 )
			{
				if ( prev_tex != c->tex )
				{
					g_renDev->TextureBind( c->tex );
					prev_tex = c->tex;
				}
				
				float	outX = x;
				if ( fixedWidth > 0 )
				{
					outX = x + (fixedWidth - c->r.size[0]) * s05;
				}
				g_renDev->DrawCharScaled( outX, pos[1], &c->r, scale[0], scale[1] );
			}

            if (fixedWidth > 0) {
                x += fixedWidth * scale[0] + letterSpace;
            } else {
                x += c->xadvance * scale[0] + letterSpace;
            }
		}

		str++;
		len--;
	}
}

//-----------------------------------------------------
//  nsFont::DrawContent
//-----------------------------------------------------
void nsFont::DrawFX( const char *str, float pos[2], float scale[2], const float c1[4], float scale2[2], const float c2[4], int len )
{
	if ( !str || _pages.empty() ) return;
	
	g_renDev->TextureBind( _pages[0] );
	void	*prev_tex = _pages[0];

	if ( !len ) len = (int)strlen( str );

	float	x = pos[0];
	while ( *str && len )
	{
		fchar_t	*c = &ch[(unsigned char)*str];
		if ( c->tex )
		{
			if ( *str != 32 )
			{
				if ( prev_tex != c->tex )
				{
					g_renDev->TextureBind( c->tex );
					prev_tex = c->tex;
				}
				
				float	cx = x + c->r.size[0] * scale[0] * 0.5f;
				float	cy = pos[1] + c->r.size[1] * scale[1] * 0.5f;

				cx = cx - c->r.size[0] * scale2[0] * 0.5f;
				cy = cy - c->r.size[1] * scale2[1] * 0.5f;
				
				g_renDev->SetColor( c2 );
				g_renDev->DrawCharScaled( cx, cy, &c->r, scale2[0], scale2[1] );
				g_renDev->SetColor( c1 );
				g_renDev->DrawCharScaled( x, pos[1], &c->r, scale[0], scale[1] );
			}
			x += c->xadvance * scale[0];
		}

		str++;
		len--;
	}
}



//------------------------------------
// GetSize: 
//------------------------------------
void nsFont::GetSize( const char *str, float size[], int len )
{
	size[0] = size[1] = 0;
	if ( !str ) return;
	if ( !len ) len = (int)strlen( str );

	while ( *str && len )
	{
        auto &info = ch[(unsigned char)*str];
        size[0] += info.xadvance;

		str++;
		len--;
	}

	size[1] = _lineHeight;
}

void nsFont::GetBounds( const char *str, nsRect &bounds) {
    nsVec3 size;
    GetSize(str, size, (int)strlen(str));

    bounds.x = 0;
    bounds.y = 0;
    bounds.width = size.x;
    bounds.height = size.y;
}

//---------------------------------------------------------
// nsFont::DrawAlphaFX:
//---------------------------------------------------------
void nsFont::DrawAlphaFX( const char *str, float pos[2], float scale[2], const float c1[4], float lerp )
{
	if ( !str ) return;

	nsColor	col = c1;
	
	g_renDev->SetColor( col );
	g_renDev->TextureBind( _pages[0] );
	void *prev_tex = _pages[0];

	float	x = pos[0];
	while ( *str )
	{
		auto	code = (unsigned char)*str;
		fchar_t	*c = &ch[code];

		if ( c->tex )
		{
			if ( code > 32 )
			{
				if ( prev_tex != c->tex )
				{
					g_renDev->TextureBind( c->tex );
					prev_tex = c->tex;
				}

				if ( lerp >= 1 )
				{
					g_renDev->SetColor( c1 );
					g_renDev->DrawCharScaled( x, pos[1], &c->r, scale[0], scale[1] );
				}
				else if ( lerp > 0 )
				{
					col[3] = lerp;
					g_renDev->SetColor( col );
					g_renDev->DrawCharScaled( x, pos[1], &c->r, scale[0], scale[1] );
				}
				lerp -= 1.0;
			}

			x += c->xadvance * scale[0];
		}
		
		str++;
	}
}


//------------------------------------
// GetRChar: 
//------------------------------------
rchar_t* nsFont::GetRChar( uchar c )
{
	if ( ch[c].tex ) return &ch[c].r;
	return nullptr;
}

//------------------------------------
// GetCharDesc: 
//------------------------------------
void nsFont::GetCharDesc( uchar c, char_desc_t &cd )
{
	if ( !ch[c].tex ) return;
	cd.width = ch[c].r.size[0];
	cd.height = ch[c].r.size[1];
	cd.offs_x = ch[c].r.offs[0];
	cd.offs_y = ch[c].r.offs[1];
}


