#ifndef _D3D_ENUM_H_
#define _D3D_ENUM_H_

#include "local.h"

typedef struct
{
	HRESULT	code;
	const char	*desc;
} errdesc_t;

extern	errdesc_t	errdesc[];
extern	int			errcount;
/*--------------------------------------------
--------------------------------------------*/

typedef struct fmtdesc_s
{
	D3DFORMAT	fmt;
	int			bits;
	int			components;
	const char		*d3desc;
	const char		*full_desc;
	
} fmtdesc_t;

extern	fmtdesc_t	fmtdesc[];
extern	int			fmtcount;

/*--------------------------------------------
--------------------------------------------*/

typedef struct
{
	int	width;
	int	height;
	int	refresh;
}
mode_desc_t;

class CModeArray
{
public:
	CModeArray() 
	{
		m_pmode = 0;
		count = 0;
	};
	
	~CModeArray()
	{
		//Clear();	my_free - ri.mem_free - ����� ���� ��� �� ���������
	};

	void Clear()
	{
		my_free( m_pmode );
		count = 0;
		m_pmode = 0;
	};
	
	int FindUserMode( int width, int height ); //return -1 if not found
	mode_desc_t* GetMode( int index );
	bool AddMode( int width, int height, int refresh );
	int			count;		

protected:
	mode_desc_t* FindMode( int width, int height );
	
	mode_desc_t	*m_pmode;
};

/*--------------------------------------------
easy array to adding data
--------------------------------------------*/
template <class T>
class CEasyArray
{
public:
	CEasyArray() {}
	~CEasyArray() {}
	
	/*--------------------------------------------
	add_raw
	--------------------------------------------*/
	bool add_raw( T **arr, int &count )
	{
		if ( !*arr )
		{
			count = 0;
			*arr = (T*)my_malloc( sizeof(T) );
		}
		else
			*arr = (T*)my_realloc( *arr, sizeof(T) * (count + 1) );
		
		if ( !*arr ) return false;
		count++;
		
		return true;
	}

	/*--------------------------------------------
	add_data
	--------------------------------------------*/
	bool add_data( T **arr, int &count, T &data )
	{
		if ( !add_raw( arr, count ) ) return false;
		memcpy( (*arr) + (count - 1), &data, sizeof(T) );
		return true;
	}
};

char*		d3d_GetError( HRESULT hr );
bool		d3d_EnumModes( CModeArray &modes, UINT dev, IDirect3D8 *d3d );
D3DFORMAT	d3d_BackBufferFormat( UINT dev, D3DDEVTYPE type, int depth, IDirect3D8 *d3d );
D3DFORMAT	d3d_WinBackBufferFormat( UINT dev, D3DDEVTYPE type, D3DFORMAT deskfmt, IDirect3D8 *d3d );
D3DFORMAT	d3d_GetTextureFormat( int texbits, D3DDEVTYPE type, D3DFORMAT backbuff, IDirect3D8 *d3d );
D3DFORMAT	d3d_GetTextureFormatRGB( int texbits, D3DDEVTYPE type, D3DFORMAT backbuff, IDirect3D8 *d3d );
fmtdesc_t*	d3d_GetFormatDesc( D3DFORMAT fmt );


#endif