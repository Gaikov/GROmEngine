#ifndef _MYMATH_H_
#define _MYMATH_H_

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Vec3.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846f
#endif

#define	M_2PI		(2.0f * M_PI)
#define M_PI2		(M_PI / 2.0f)

inline void inc_angle( float &angle, float t )
{
	angle += t;
	if ( angle >= M_2PI ) angle = angle - M_2PI;
	else if ( angle < 0 ) angle = M_2PI + angle;
}

inline void swapf( float &a, float &b )
{
	float	tmp = a;
	a = b;
	b = tmp;
}

/*-------------------------------------------------------------------------------------
 MATH
-------------------------------------------------------------------------------------*/
inline float signrand( float range = 1.0 )
{
	return (rand() / float(RAND_MAX >> 1) - 1.0f) * range;
}

inline float unsignrand( float range = 1.0 )
{
	return (float(rand()) / float(RAND_MAX)) * range;
}

/*inline int roundf( float val )
{
	int		v = (int)val;
	float	rem = val - v;
	if ( rem >= 0.5f ) v++;
	return v;
}//*/

/*--------------------------------------------
--------------------------------------------*/
struct vec3_t
{
	float	x, y, z;

	operator float* ()
	{
		return &x;
	}

    operator nsVec3() const
	{
		return nsVec3( x, y, z );
	}

    void operator = (const nsVec3 &v) {
        x = v.x;
        y = v.y;
        z = v.z;
    }

	inline vec3_t& operator += (const vec3_t v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	inline vec3_t operator - (const vec3_t& v)
	{
		vec3_t res;
		res.x = x - v.x;
		res.y = y - v.y;
		res.z = z - v.z;
		return res;
	}
};

inline char *vtos( vec3_t &v )
{
	static char	str[64];
	sprintf( str, "(%f %f %f)", v.x, v.y, v.z );
	return str;
}

typedef struct matrix_s
{
	union
	{
		float m[4][4];

		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
	};
	
	operator float* ()
	{
		return &_11;
	}
}
matrix_t;

/*--------------------------------------------
math operations
--------------------------------------------*/
inline vec3_t vec3( float x, float y, float z )
{
	vec3_t	v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

inline vec3_t vec3( const nsVec3 &v )
{
	vec3_t	_v = { v.x, v.y, v.z };
	return _v;
}

//adding vectors
inline vec3_t operator + ( const vec3_t &a, const vec3_t &b )
{
	vec3_t	c;
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
	return c;
}

//subtract vectors
inline vec3_t operator - ( const vec3_t &a, const vec3_t &b )
{
	vec3_t	c;
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;

	return c;
}

inline vec3_t operator - ( vec3_t &a )
{
	vec3_t	b;
	b.x = -a.x;
	b.y = -a.y;
	b.z = -a.z;
	return b;
}

//mul by scalar
inline vec3_t operator * ( const vec3_t &a, const float t )
{
	vec3_t	c;
	c.x = a.x * t;
	c.y = a.y * t;
	c.z = a.z * t;

	return c;
}

inline vec3_t operator * ( const float t, const vec3_t &a )
{
	vec3_t	c;
	c.x = a.x * t;
	c.y = a.y * t;
	c.z = a.z * t;

	return c;
}

inline void operator *= ( vec3_t &a, float t )
{
	a.x *= t;
	a.y *= t;
	a.z *= t;
}

inline vec3_t operator / ( const vec3_t &a, const float t )
{
	vec3_t	c;
	c.x = a.x / t;
	c.y = a.y / t;
	c.z = a.z / t;

	return c;
}

inline vec3_t operator / ( const float t, const vec3_t &a )
{
	vec3_t	c;
	c.x = a.x / t;
	c.y = a.y / t;
	c.z = a.z / t;

	return c;
}

inline vec3_t& operator /= ( vec3_t &a, float t )
{
	a.x /= t;
	a.y /= t;
	a.z /= t;
	return a;
}

//cross product
inline vec3_t vec3cross( const vec3_t &a, const vec3_t &b )
{
	vec3_t	c;
	c.x = a.y * b.z - a.z * b.y;
	c.y = a.z * b.x - a.x * b.z;
	c.z = a.x * b.y - a.y * b.x;
	return c;
}

//dot product
inline float vec3dot( const vec3_t &a, const vec3_t &b )
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

//vector length
inline float vec3len( const vec3_t &a )
{
	return sqrtf( vec3dot( a, a ) );
}

//vector length squared
inline float vec3sqlen( const vec3_t &a )
{
	return vec3dot( a, a );
}

//linear interpolation
inline vec3_t vec3lerp( vec3_t a, vec3_t b, float t )
{
	return a + ( b - a ) * t;
}

//normalize
inline void vec3norm( vec3_t &a )
{
	float	ilen = 1.0f / sqrtf( vec3dot( a, a ) );
	a *= ilen;
}

inline void operator *= ( vec3_t &v, matrix_t &m )
{
	vec3_t	r;
	r.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	r.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	r.z = v.x * m._13 + v.y * m._23 + v.z * m._33;

	v = r;
}//*/

inline vec3_t vec3rand()
{
	vec3_t	v;
	v.x = signrand();
	v.y = signrand();
	v.z = signrand();
	
	vec3norm( v );
	return v;
}
/*--------------------------------------------
vector 2D
--------------------------------------------*/
struct vec2
{
	float	x, y;

	inline vec2() {};
	inline vec2( float _x, float _y )
	{
		x = _x;
		y = _y;
	}
	
	inline float len()
	{
		return sqrtf( x * x + y * y );
	}
	
	inline void norm()
	{
		float	l = len();
		x /= l;
		y /= l;
	}

	inline vec2 right()
	{
		return vec2( y, -x );
	}

	inline vec2 left()
	{
		return vec2( -y, x );
	}


	inline vec2& operator += ( vec2 a )
	{
		x += a.x;
		y += a.y;
		return *this;
	}

	inline vec2& operator -= ( vec2 a )
	{
		x -= a.x;
		y -= a.y;
		return *this;
	}

	inline operator float*()
	{
		return &x;
	}
};

inline vec2 operator + ( vec2& a, vec2& b )
{
	return vec2( a.x + b.x, a.y + b.y );
}

inline vec2 operator - ( vec2& a, vec2& b )
{
	return vec2( a.x - b.x, a.y - b.y );
}

inline vec2 operator - ( vec2& a )
{
	return vec2( -a.x, -a.y );
}

inline vec2 operator * ( vec2& a, float t )
{
	return vec2( a.x * t, a.y * t );
}

inline vec2 operator * ( float t, vec2& a )
{
	return vec2( a.x * t, a.y * t );
}

/*--------------------------------------------
--------------------------------------------*/
inline void matIdent( matrix_t &m )
{
	m._11 = m._22 = m._33 = m._44 = 1.0;
	
	m._12 = m._13 = m._14 = 
	m._21 = m._23 = m._24 = 
	m._31 = m._32 = m._34 = 
	m._41 = m._42 = m._43 = 0;
}

inline void matScale( matrix_t &m, float x, float y, float z )
{
	matIdent( m );
	m._11 = x;
	m._22 = y;
	m._33 = z;
}

inline void matRotY( matrix_t &m, float angle )
{
	matIdent( m );
	float	c = cosf( -angle );
	float	s = sinf( -angle );

	m._11 = c;	m._13 = s;
	m._31 = -s;	m._33 = c;
}

inline void matRotZ( matrix_t &m, float angle )
{
	matIdent( m );
	float	c = cosf( -angle );
	float	s = sinf( -angle );

	m._11 = c;	m._12 = s;
	m._21 = -s;	m._22 = c;
}

inline void matRotX( matrix_t &m, float angle )
{
	matIdent( m );
	float	c = cosf( -angle );
	float	s = sinf( -angle );

	m._22 = c;	m._23 = s;
	m._32 = -s;	m._33 = c;
}

inline void matRotAxis( matrix_t &m, vec3_t &v, float angle )
{
	m._41 = m._42 = m._43 = m._14 = m._24 = m._34 = 0;
	m._44 = 1;
	
	float	c = cosf( -angle );
	float	s = sinf( -angle );

	m._11 = v.x * v.x + ( 1 - v.x * v.x ) * c;
	m._12 = v.x * v.y * ( 1 - c ) + v.z * s;
	m._13 = v.x * v.z * ( 1 - c ) - v.y * s;
	m._21 = v.x * v.y * ( 1 - c ) - v.z * s;

	m._22 = v.y * v.y + ( 1 - v.y * v.y ) * c;
	m._23 = v.y * v.z * ( 1 - c ) + v.x * s;
	m._31 = v.x * v.z * ( 1 - c ) + v.y * s;
	m._32 = v.y * v.z * ( 1 - c ) - v.x * s;
	
	m._33 = v.z * v.z + ( 1 - v.z * v.z ) * c;
}

inline void matMove( matrix_t &m, float x, float y, float z )
{
	matIdent( m );
	m._41 = x;
	m._42 = y;
	m._43 = z;
}

inline void matMove( matrix_t &m, vec3_t &v )
{
	matIdent( m );
	m._41 = v.x;
	m._42 = v.y;
	m._43 = v.z;
}

inline vec3_t matPos( matrix_t &m )
{
	return vec3( m._41, m._42, m._43 );
}

inline matrix_t& operator* ( matrix_t &a, matrix_t &b )
{
	static	matrix_t	res;
	float sum;
	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
		{
			sum = 0;		
			for ( int k = 0; k < 4; k++ )
				sum += a.m[i][k] * b.m[k][j];
			res.m[i][j] = sum;
		}

	return res;
}

/*--------------------------------------------
planes
--------------------------------------------*/
typedef struct
{
	vec3_t	n;
	float	dist;
}
plane_t;

#endif