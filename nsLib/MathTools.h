#pragma once

#include "headers.h"
#include "math/Random.h"

#ifndef M_PI
#define	M_PI 3.1415926535897932384626433832795f
#endif

#define M_2PI (2.0f * M_PI)
#define M_PI2 (M_PI / 2.0f)

float	Math_SinUnit( float one );
int		Math_Round( float v );
bool	Math_Clamp( float &v, float min, float max );
void	Math_Scale( float &v, float fromStart, float fromEnd, float toStart, float toEnd );
float	Math_Rand( float max );
float	Math_RandRange( float min, float max );

//---------------------------------------------------------
// nsMath:
//---------------------------------------------------------
struct nsMath
{
	/* ������� ��������� ����� �� ����� p1, p2, � ����� in, ��������� � out,
    ���������� ���������� �� ������� out ������� �� ��������� ����� p1 ��� p2,
    < 0 - ����� ������ �������. */
	static inline float		Absf( float val );
	static inline double	Absd( double val );

    // ���������� true ���� ����� ����� ������ ���� �� ����� ������������
    static inline float		Clamp11( float val );	//����� -1 �� 1

    static bool				IsValidTriangle( int i1, int i2, int i3 ); // ��������� ������� �� ����������

	static inline float		RandAngle();	//���������� ��������� �������� ���� � ��������

	static inline float		Min( float x1, float x2 );
	static inline float		Max( float x1, float x2 );
	static inline void		MinMax( float x1, float x2, float &minRes, float &maxRes );
	static bool             InRange( float a, float min, float max);
    static inline float     Sign( float v ) { return v < 0 ? -1 : 1; };
    static inline float     Clamp(float v, float min, float max);

    static inline float     Lerp(float a, float b, float t);
    static float            Move(float from, float to, float delta);
    static float            MoveExp(float from, float to, float speedScale, float deltaTime);
	static inline float		ToRad(float degrees);
    static inline float     ToDeg(float radians);
    static inline float     RandRange(float min, float max);
    static inline float     Random();
	static inline float		NormalizeAngle(float angle);
	static inline float		ClosestAngle(float from, float to);
};

//---------------------------------------------------------
// Math_SinUnit: 
//---------------------------------------------------------
inline float Math_SinUnit( float one )
{
	return sinf( M_PI * (one - 0.5f) ) * 0.5f + 0.5f;
}

//---------------------------------------------------------
// Math_Round: 
//---------------------------------------------------------
inline int Math_Round( float val )
{
	int		v = (int)val;
	float	rem = val - v;
	if ( rem >= 0.5f ) v++;
	return v;
}

//---------------------------------------------------------
// Math_Clamp: 
//---------------------------------------------------------
inline bool	Math_Clamp( float &v, float min, float max )
{
	if ( v < min )
	{
		v = min;
		return true;
	}
	else if ( v > max )
	{
		v = max;
		return true;
	}
	return false;
}

//---------------------------------------------------------
// Math_Scale: 
//---------------------------------------------------------
inline void Math_Scale( float &v, float fromStart, float fromEnd, float toStart, float toEnd )
{
	v = toStart + (v - fromStart) * (toEnd - toStart) / (fromEnd - fromStart);
}

//---------------------------------------------------------
// Math_Rand: 
//---------------------------------------------------------
inline float Math_Rand( float max )
{
	return float(rand()) * max / float(RAND_MAX);
}

//---------------------------------------------------------
// Math_RandRange: 
//---------------------------------------------------------
inline float Math_RandRange( float min, float max )
{
	if ( min > max )
	{
		float	tmp = min;
		min = max;
		max = tmp;
	}

	return min + Math_Rand( 1 ) * (max - min);
}

//---------------------------------------------------------
// nsMath::Absf:
//---------------------------------------------------------
float nsMath::Absf( float val )
{
	if ( val < 0 ) val = -val;
	return val;
}

//-----------------------------------------------------
// nsMath::Absd:
//-----------------------------------------------------
double nsMath::Absd( double val )
{
	if ( val < 0 ) val = -val;
	return val;
}

//-----------------------------------------------------
// nsMath::Clamp11:
//-----------------------------------------------------
float nsMath::Clamp11( float val )
{
    return Clamp(val, -1, 1);
}

//-----------------------------------------------------
// nsMath::RandAngle:
//-----------------------------------------------------
float nsMath::RandAngle()
{
	return nsRandom::Random() * static_cast<float>(M_2PI);
}

//-----------------------------------------------------
// nsMath::Min:
//-----------------------------------------------------
float nsMath::Min( float x1, float x2 )
{
	return x1 < x2 ? x1 : x2;
}

//-----------------------------------------------------
// nsMath::Max:
//-----------------------------------------------------
float nsMath::Max( float x1, float x2 )
{
	return x1 > x2 ? x1 : x2;
}

//-----------------------------------------------------
// nsMath::MinMax:
//-----------------------------------------------------
void nsMath::MinMax( float x1, float x2, float &minRes, float &maxRes )
{
	if ( x1 < x2 )
	{
		minRes = x1;
		maxRes = x2;
	}
	else
	{
		minRes = x2;
		maxRes = x1;
	}
}

float nsMath::Clamp(float v, float min, float max) {
    if (v < min) {
        v = min;
    } else if (v > max) {
        v = max;
    }
    return v;
}

float nsMath::Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float nsMath::ToRad(const float degrees) {
    return degrees * float(M_PI) / 180.0f;
}

float nsMath::ToDeg(float radians) {
    return radians * 180.0f / M_PI;
}

float nsMath::RandRange(float min, float max) {
    return Math_RandRange(min, max);
}

float nsMath::Random() {
    return float(rand()) / float(RAND_MAX);
}

float nsMath::NormalizeAngle(float angle) {

	angle = remainder(angle, static_cast<float>(M_2PI));
	if (angle < 0)
	{
		angle = M_2PI + angle;
	}
	return angle;
}

float nsMath::ClosestAngle(float from, float to) {
	from = NormalizeAngle(from);
	to = NormalizeAngle(to);

	const auto delta = to - from;
	if (delta < -M_PI)
	{
		to += M_2PI;
	}
	else if (delta > M_PI)
	{
		to -= M_2PI;
	}
	return to;
}
