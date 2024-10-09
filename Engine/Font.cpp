// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Font.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "Font.h"
#include "nsLib/log.h"
#include "Core/Memory.h"
#include "Core/ParseFile.h"
#include "Core/StructUt.h"
#include "Core/Config.h"
#include "nsLib/color.h"
#include "nsLib/StrTools.h"
#include "RenManager.h"

//---------------------------------------------------------
// nsFont::Load: 
//---------------------------------------------------------
bool nsFont::Load( const char *fileName )
{
	if ( !StrCheck( fileName ) ) return false;
	
	LogPrintf( PRN_ALL, "loading font: %s\n", fileName );
	
	nsParseFile		pf;
	script_state_t	*ss = pf.BeginFile( fileName );
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
				AddToArray( &tex_list, tex_count, tex );
		}
		while ( ps_var_next( ss ) );
		ps_block_end( ss );
	}
	
	if ( !tex_count )
	{
		LogPrintf( PRN_ALL, "WARNING: font textures not loaded!\n" );
		return false;
	}
	
	//-------------------------------------------
	int	i = 0;
	avg_height = 0;
	float	min_h = 0;
	float	max_h = 0;
	if ( ps_block_begin( ss, "chars_data" ) 
		&& ps_block_begin( ss, "char" ) )
	{
		do
		{
			int	code;
			ps_var_begin( ss, "code" );
			code = (int)ps_var_f( ss );
			
			int	tex_id;
			ps_var_begin( ss, "tex_id" );
			tex_id = (int)ps_var_f( ss );
			ITexture *tex = tex_id >= 0 && tex_id < tex_count ? tex_list[tex_id] : 0;
			
			bool	init_max = true;
			
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
	avg_height = max_h - min_h;
	
	id = RES_FONT;
	m_fileName = fileName;
	return true;
}

//------------------------------------
// Free: 
//------------------------------------
void nsFont::Free()
{
	if ( tex_list )
	{
		for ( int i = 0; i < tex_count; i++ )
			g_renDev->TextureRelease( tex_list[i] );
		my_free( tex_list );
	}
	memset( this, 0, sizeof(nsFont) );
}

//------------------------------------
// DrawContent:
//------------------------------------
void nsFont::Draw( const char *str, float pos[], float scale[], const float c[4], int len, float fixedWidth )
{
	if ( !str || !tex_list ) return;
	
	g_renDev->SetColor( c );
	g_renDev->TextureBind( tex_list[0] );
	void	*prev_tex = tex_list[0];

	if ( !len ) len = strlen( str );

	float	x = pos[0];
	float	s05 = scale[0] * 0.5f;
	while ( *str && len )
	{
		auto	chr = (unsigned char)*str;
/*#ifdef _DEBUG
		if ( chr > 128 )
		{
			int i = 0;
		}
#endif//*/

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
				if ( fixedWidth )
				{
					/*if ( chr != ':' )
						outX = x + (fixedWidth - c->r.size[0]) * s05;
					else
						outX = x + (c->r.size[0] - c->r.size[0]) * s05;//*/
					outX = x + (fixedWidth - c->r.size[0]) * s05;
						
				}
				g_renDev->DrawCharScaled( outX, pos[1], &c->r, scale[0], scale[1] );
			}
			
			if ( fixedWidth )
			{
				/*if ( chr != ':' )
					x += fixedWidth * scale[0];
				else
					x += c->r.size[0] * scale[0];//*/
				x += fixedWidth * scale[0];
			}
			else
				x += (c->r.offs[0] + c->r.size[0]) * scale[0];
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
	if ( !str || !tex_list ) return;
	
	g_renDev->TextureBind( tex_list[0] );
	void	*prev_tex = tex_list[0];

	if ( !len ) len = strlen( str );

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
			x += (c->r.offs[0] + c->r.size[0]) * scale[0];
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
	if ( !len ) len = strlen( str );

	while ( *str && len )
	{
		rchar_t	*c = &ch[(unsigned char)*str].r;
		size[0] += (c->offs[0] + c->size[0]);
		str++;
		len--;
	}

	size[1] = avg_height;
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
	g_renDev->TextureBind( tex_list[0] );
	void *prev_tex = tex_list[0];

	float	x = pos[0];
	while ( *str )
	{
		unsigned char	code = (unsigned char)*str;
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

			x += (c->r.offs[0] + c->r.size[0]) * scale[0];
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
	return 0;
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

//------------------------------------------------------
// nsFont::TestFont: 
//------------------------------------------------------
nsVar			*ft_file = 0;
nsVar			*ft_str = 0;
nsVar			*ft_pos_x = 0;
nsVar			*ft_pos_y = 0;
static nsFont	testFont;

void nsFont::TestFont()
{
	/*g_renDev->SetRenderMode( RM_ORTHO_640 );
	g_renDev->SetAlphaMode( ALPHA_GLASS );//*/

	if ( strlen( ft_file->String() ) 
		&& testFont.m_fileName != ft_file->String() )
	{
		testFont.Free();
		if ( !testFont.Load( (char*)ft_file->String() ) )
			return;
	}
	
	if ( !testFont.m_fileName.Length() ) return;

	float	pos[2] = { ft_pos_x->Value(), ft_pos_y->Value() };

	float	s[2] = { 1, 1 };
	testFont.Draw( (char*)ft_str->String(), pos, s, nsColor::white, 0, 0 );
}

//------------------------------------------------------
// nsFont::TestInit: 
//------------------------------------------------------
void nsFont::TestInit()
{
	ft_file = g_cfg->RegVar( "ft_file", "", 0 );
	ft_str = g_cfg->RegVar( "ft_str", "", 0 );
	ft_pos_x = g_cfg->RegVar( "ft_pos_x", "100", 0 );
	ft_pos_y = g_cfg->RegVar( "ft_pos_y", "100", 0 );
}

//------------------------------------------------------
// nsFont::TestFree: 
//------------------------------------------------------
void nsFont::TestFree()
{
	testFont.Free();
}
