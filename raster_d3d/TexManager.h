#ifndef _TEXMANAGER_H_
#define _TEXMANAGER_H_

#include "Local.h"

#define	RES_TEXTURE	0x1111

class nsTexture : public ITexture
{
friend class CTexManager;

public:
	nsTexture			*next, *prev; //to manage textures

private:
	int					id;		//0x1111 - is texture, simple protection
	bool				bitmap;	//true - use bitmap data, false - use file data
	
	//file texture
	nsString			m_fileName;
	
	//bitmap texture
	uchar				*data;
	texfmt_t			fmt;

	//common fields
	IDirect3DTexture8	*tex;
	bool				use_mipmap;
	int					width, height;
	int					ref_count;

private:
	nsTexture();

	virtual void GetSize( int &w, int &h );
};

/*--------------------------------------------
--------------------------------------------*/

class CTexManager  
{
public:
	CTexManager();

	void		release( bool bWithUserData );
	bool		init();
	
	nsTexture*	TextureLoad( const char *fileName, bool mipmap, texfmt_t fmt );
	nsTexture*	TextureGenerate( int width, int height, const void *data, texfmt_t fmt, bool mipmap );
	void		TextureRelease( nsTexture *texture );
	void		TextureBind( nsTexture *tex );

	static void tm_reload_textures_f( int argc, const char *argv[] );
	static void tm_reload_texture_f( int argc, const char *argv[] );
	static void tm_list_f( int argc, const char *argv[] );
	
	bool		ReloadTextures();
	nsTexture*	FindTexture( const char *filename, bool mipmap, texfmt_t fmt );
	bool		ReloadTexture( nsTexture *desc );

	nsTexture	*m_texList;
	
	D3DFORMAT	m_texfmt;
	D3DFORMAT	m_texfmt_rgb;

	nsVec3		tg_dir, tg_up, tg_right;	//texgen vectors
	nsVec3		tg_pos;
	float		tg_size;

protected:
	bool		ReloadFileTex( nsTexture *desc );
	bool		ReloadBitmapTex( nsTexture *desc );

private:
	nsTexture	*m_currTex;
};

extern	CTexManager	tm;

#endif
