#ifndef _COLOR_H_
#define _COLOR_H_

#include "headers.h"
#include "comm_types.h"
#include "MathTools.h"

struct nsColor
{
	static const nsColor white;
	static const nsColor black;
	static const nsColor red;
	static const nsColor green;
	static const nsColor blue;
	static const nsColor yellow;
	static const nsColor gray;
	static const nsColor darkGray;

	union
	{
		struct 
		{
			float	r, g, b, a;
		};
		float	color[4];
	};
	
	inline nsColor()
	{
		r = g = b = a = 1;
	}
	
	inline nsColor( float _r, float _g, float _b, float _a = 1 )
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

    inline nsColor( const nsColor& c, float alpha) {
        r = c.r;
        g = c.g;
        b = c.b;
        a = alpha;
    }
	
	inline nsColor( const float c[4] )
	{
		*this = c;
	}
	
	inline operator const float*() const
	{
		return color;
	}

    inline operator float* ()
    {
    	return color;
    }
	
	inline nsColor& operator = ( const float c[4] )
	{
		if ( !c ) 
			*this = nsColor();
		else
		{
			color[0] = c[0];
			color[1] = c[1];
			color[2] = c[2];
			color[3] = c[3];
		}
		return *this;
	}
	
	inline dword ToARGB() const
	{
		return ToARGB( color );
	}

	static inline dword ToARGB( const float *c );
	void FromRGBA(uint32_t c);
	void CopyTo( float c[4]) const;

	inline void Rand()
	{
		r = float(rand() % 1000) / 1000.0f;
		g = float(rand() % 1000) / 1000.0f;
		b = float(rand() % 1000) / 1000.0f;
		a = 1;
		color[ rand() % 3 ] = 1;
	}

	inline void FromLerp( const nsColor &a, const nsColor &b, float t );
	inline void operator *= ( const nsColor &other );

    void FromBytes(byte r, byte g, byte b, byte a = 255);
};

//---------------------------------------------------------
// nsColor::ToARGB:
//---------------------------------------------------------
dword nsColor::ToARGB( const float *c )
{
	if ( !c ) c = white;
	return dword(
       	(byte(255.0f * c[3]) << 24) |
		(byte(255.0f * c[2]) << 16) |
		(byte(255.0f * c[1]) << 8) |
		byte(255.0f * c[0]) );
}

//---------------------------------------------------------
// nsColor::FromLerp: 
//---------------------------------------------------------
void nsColor::FromLerp( const nsColor &a, const nsColor &b, float t )
{
	for ( int i = 0; i < 4; ++i )
		color[i] = nsMath::Lerp( a.color[i], b.color[i], t );
}

//---------------------------------------------------------
// nsColor::operator *=: 
//---------------------------------------------------------
void nsColor::operator *= ( const nsColor &other )
{
	r *= other.r;
	g *= other.g;
	b *= other.b;
	a *= other.a;
}


#endif