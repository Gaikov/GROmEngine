#ifndef _LINE2_H_
#define _LINE2_H_

#include "Vec2.h"
#include "MathTools.h"

class nsLine2
{
public:
	nsVec2	norm;
	float	dist;
	nsVec2	v1, v2;
	nsVec2	ray;	//vector from v1 to v2
	float	length;

public:
	nsLine2( const nsVec2 &p1, const nsVec2 &p2 );
	void	FromPoints( const nsVec2 &p1, const nsVec2 &p2 );
	bool	LineCross( const nsVec2 &from, const nsVec2 &to, nsVec2 &res );
	float	PointOffset( const nsVec2 &point );
	bool	PointInsideLine( const nsVec2 &point );
	float	ClosestPoint( const nsVec2 &point, nsVec2 &res );
};

//-----------------------------------------------------
// nsLine2:
//-----------------------------------------------------
inline nsLine2::nsLine2( const nsVec2 &p1, const nsVec2 &p2 )
{
	FromPoints( p1, p2 );
}

//-----------------------------------------------------
// FromPoints:
//-----------------------------------------------------
inline void nsLine2::FromPoints( const nsVec2 &p1, const nsVec2 &p2 )
{
	v1 = p1;
	v2 = p2;
	ray = p2 - p1;
	length = ray.Length();
	ray /= length;
	norm = ray.GetLeft();
	dist = -norm.Dot( p1 );
}

//-----------------------------------------------------
// nsLine2::PointOffset:
//-----------------------------------------------------
inline float nsLine2::PointOffset( const nsVec2 &point )
{
	return norm.Dot( point ) + dist;
}

//-----------------------------------------------------
// nsLine2::PointOnLine:
//-----------------------------------------------------
inline bool	nsLine2::PointInsideLine( const nsVec2 &point )
{
	nsVec2	toPoint = point - v1;
	float	dot = ray.Dot( toPoint );
	return dot >= 0 && dot <= length;
}

//-----------------------------------------------------
// nsLine2::ClosestPoint:
//-----------------------------------------------------
inline float nsLine2::ClosestPoint( const nsVec2 &point, nsVec2 &res )
{
	nsVec2	toPoint = point - v1;
	float	dot = ray.Dot( toPoint );
	res = v1 + ray * dot;
	return dot;	//отклонение от начальной точкм линии
}


#endif
