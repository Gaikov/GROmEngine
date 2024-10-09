#ifndef _MATRIX4_H_
#define _MATRIX4_H_

#include "Vec3.h"
#include "MathTools.h"

//-----------------------------------------------------
//  struct nsMatrix:  
//-----------------------------------------------------
struct nsMatrix
{
public:
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

	static const nsMatrix	identity;

public:
	inline nsMatrix() {}

	//-----------------------------------------------------
	//  explicit nsMatrix( float scale ):  
	//-----------------------------------------------------
	inline explicit nsMatrix( float scale )
	{
		Scale( scale, scale, scale );
	}

	//-----------------------------------------------------
	//  Identity:  
	//-----------------------------------------------------
	inline void Identity()
	{
		_11 = _22 = _33 = _44 = 1;
		_12 = _13 = _14 = _21 = _23 = _24 = _31 = _32 = _34 = _41 = _42 = _43 = 0;
	}

	//-----------------------------------------------------
	//  Scale:  
	//-----------------------------------------------------
	inline void Scale( float x, float y, float z )
	{
		_11 = x;
		_22 = y;
		_33 = z;
		_44 = 1;

		_12 = _13 = _14 = _21 = _23 = _24 = _31 = _32 = _34 = _41 = _42 = _43 = 0;
	}

	//-----------------------------------------------------
	//  AxisRotate:  
	//-----------------------------------------------------
	inline void AxisRotate( const nsVec3 &v, float angle )
	{
		_41 = _42 = _43 = _14 = _24 = _34 = 0;
		_44 = 1;
		
		float	c = (float)cos( -angle );
		float	s = (float)sin( -angle );

		_11 = v.x * v.x + ( 1 - v.x * v.x ) * c;
		_12 = v.x * v.y * ( 1 - c ) + v.z * s;
		_13 = v.x * v.z * ( 1 - c ) - v.y * s;
		_21 = v.x * v.y * ( 1 - c ) - v.z * s;

		_22 = v.y * v.y + ( 1 - v.y * v.y ) * c;
		_23 = v.y * v.z * ( 1 - c ) + v.x * s;
		_31 = v.x * v.z * ( 1 - c ) + v.y * s;
		_32 = v.y * v.z * ( 1 - c ) - v.x * s;
		
		_33 = v.z * v.z + ( 1 - v.z * v.z ) * c;
	}

	//-----------------------------------------------------
	//  nsMatrix& operator =:  
	//-----------------------------------------------------
	inline nsMatrix& operator = ( const float _m[4][4] )
	{
		for ( int i = 0; i < 4; i++ )
			for ( int j = 0; j < 4; j++ )
				m[i][j] = _m[i][j];
		return *this;
	}

	//-----------------------------------------------------
	//  nsMatrix& operator =:  
	//-----------------------------------------------------
	inline nsMatrix& operator = ( const float* _m )
	{
		float	*mat = &_11;
		for ( int i = 0; i < 16; i++, mat++, _m++ )
			*mat = *_m;
		return *this;
	}

	//-----------------------------------------------------
	//  RotateY:  
	//-----------------------------------------------------
	inline void RotateY( float angle )
	{
		Identity();
		float	c = (float)cos( -angle );
		float	s = (float)sin( -angle );

		_11 = c;	_13 = s;
		_31 = -s;	_33 = c;
	}

	//-----------------------------------------------------
	//  RotateZ:  
	//-----------------------------------------------------
	inline void RotateZ( float angle )
	{
		Identity();
		float	c = (float)cos( -angle );
		float	s = (float)sin( -angle );

		_11 = c;	_12 = s;
		_21 = -s;	_22 = c;
	}

	//-----------------------------------------------------
	//  RotateX:  
	//-----------------------------------------------------
	inline void RotateX( float angle )
	{
		Identity();
		float	c = (float)cos( -angle );
		float	s = (float)sin( -angle );

		_22 = c;	_23 = s;
		_32 = -s;	_33 = c;
	}

	inline nsVec3 GetViewY() const
	{
		return nsVec3( _12, _22, _32);
	}

	inline nsVec3 GetViewX() const
	{
		return nsVec3(_11, _21, _31);
	}

	inline nsVec3 GetViewZ() const
	{
		return nsVec3(_13, _23, _33);
	}
	
	inline void SetOrthoRH( float left, float right, float bottom, float top, float minZ, float maxZ );
	inline void SetOrthoDimRH( float width, float height );
	inline void SetProjFovRH( float fov, float aspect, float zNear, float zFar );
	inline void SetLookAtRH( const nsVec3 &pos, const nsVec3 &at, const nsVec3 &up );
	inline void GetY( nsVec3 &v );
	inline void SetPos( const nsVec3 &pos );
	inline operator const float*() const;

	inline void Project( nsVec3 &v );
};

inline nsMatrix& operator * ( const nsMatrix &a, const nsMatrix &b )
{
	static	nsMatrix	res;
	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
		{
			float sum = 0;
			for ( int k = 0; k < 4; k++ )
				sum += a.m[i][k] * b.m[k][j];
			res.m[i][j] = sum;
		}

	return res;
}

inline void operator *= ( nsVec3 &v, const nsMatrix &m )
{
	nsVec3	r;
	r.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	r.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	r.z = v.x * m._13 + v.y * m._23 + v.z * m._33;

	v = r;
}

//---------------------------------------------------------
// nsMatrix::operator: 
//---------------------------------------------------------
nsMatrix::operator const float*() const
{
	return &_11;
}

//---------------------------------------------------------
// nsMatrix::SetPos: 
//---------------------------------------------------------
void nsMatrix::SetPos( const nsVec3 &pos )
{
	_41 = pos.x;
	_42 = pos.y;
	_43 = pos.z;
}

//---------------------------------------------------------
// nsMatrix::SetOrthoRH: 
//---------------------------------------------------------
//2/(r-l)      0            0           0
//0            2/(t-b)      0           0
//0            0            1/(zn-zf)   0
//(l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1
void nsMatrix::SetOrthoRH( float left, float right, float bottom, float top, float minZ, float maxZ )
{
	_11 = 2 / (right - left);	_12 = _13 = _14 = 0;
	_21 = 0; _22 = 2 / (top - bottom); _23 = _24 = 0;
	_31 = _32 = 0; _33 = 1 / (minZ - maxZ); _34 = 0;
	
	_41 = (left + right) / (left - right);
	_42 = (top + bottom) / (bottom - top);
	_43 = minZ / (minZ - maxZ);
	_44 = 1;
}

//---------------------------------------------------------
// nsMatrix::SetOrthoDimRH: 
//---------------------------------------------------------
void nsMatrix::SetOrthoDimRH( float width, float height )
{
	SetOrthoRH( 0, width, 0, height, -1, 1 );
}

//---------------------------------------------------------
// nsMatrix::SetProjFovRH: 
//---------------------------------------------------------
void nsMatrix::SetProjFovRH( float fov, float aspect, float zNear, float zFar )
{
	float	f = cosf( fov / 2 ) / sinf( fov / 2 );

	_11 = f / aspect; _12 = _13 = _14 = 0;
	_21 = 0; _22 = f; _23 = _24 = 0;
	_31 = _32 = 0; _33 = (zFar + zNear) / (zNear - zFar); _34 = -1;
	_41 = _42 = 0; _43 = 2 * zNear * zFar / (zNear - zFar); _44 = 0;
}

//---------------------------------------------------------
// nsMatrix::SetLookAtRH: 
//---------------------------------------------------------
void nsMatrix::SetLookAtRH( const nsVec3 &pos, const nsVec3 &at, const nsVec3 &up )
{
	nsVec3	z = (pos - at); z.Norm();
	nsVec3	x = up.Cross( z ); x.Norm();
	nsVec3	y = z.Cross( x ); y.Norm();

	_14 = _24 = _34 = 0; _44 = 1;

	_11 = x.x;	_12 = y.x;	_13 = z.x;
	_21 = x.y;	_22 = y.y;	_23 = z.y;
	_31 = x.z;	_32 = y.z;	_33 = z.z;

	_41 = -x.Dot( pos );
	_42 = -y.Dot( pos );
	_43 = -z.Dot( pos );
}

//---------------------------------------------------------
// nsMatrix::GetY: 
//---------------------------------------------------------
void nsMatrix::GetY( nsVec3 &v )
{
	v.x = _21;
	v.y = _22;
	v.z = _23;
}

//---------------------------------------------------------
// nsMatrix::Project: 
//---------------------------------------------------------
void nsMatrix::Project( nsVec3 &v )
{
	nsVec3	res;
	float	w;

	res.x = v.x * _11 + v.y * _21 + v.z * _31 + _41;
	res.y = v.x * _12 + v.y * _22 + v.z * _32 + _42;
	res.z = v.x * _13 + v.y * _23 + v.z * _33 + _43;
	w = 1.0f / (v.x * _14 + v.y * _24 + v.z * _34 + _44);

	res *= w;
	v = res;
}

#endif