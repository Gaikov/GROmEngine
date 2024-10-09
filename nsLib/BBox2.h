#ifndef _BBOX2_H_
#define _BBOX2_H_

#include "Vec2.h"
#include "MathTools.h"

//---------------------------------------------------------
// nsBBox2:
//---------------------------------------------------------
class nsBBox2
{
public:
	nsVec2	m_min, m_max;

public:
	inline nsBBox2() {}
	inline nsBBox2( float minX, float minY, float maxX, float maxY );
	inline nsBBox2( nsVec2 &_min, nsVec2 &_max );
	inline nsBBox2( float size );
	inline bool		PointInBox( float x, float y, float boxX, float boxY ) const;
	inline bool		PointInBox( const nsVec2 &pt, const nsVec2 &boxPos ) const;
	inline bool		Intersect( const nsBBox2 &box ) const;
	inline float	Width() const { return m_max.x - m_min.x; } 
	inline float	Height() const { return m_max.y - m_min.y; }
};


//---------------------------------------------------------
// nsBBox2::nsBBox2:
//---------------------------------------------------------
nsBBox2::nsBBox2( float minX, float minY, float maxX, float maxY ) :
	m_min( minX, minY ),
	m_max( maxX, maxY )
{}


//---------------------------------------------------------
// nsBBox2::nsBBox2:
//---------------------------------------------------------
nsBBox2::nsBBox2( nsVec2 &_min, nsVec2 &_max ) :
	m_min( _min ),
	m_max( _max )
{}

//---------------------------------------------------------
// nsBBox2::nsBBox2:
//---------------------------------------------------------
nsBBox2::nsBBox2( float size ) :
	m_min( -size / 2, -size / 2 ),
	m_max( size / 2, size / 2 )
{}

//---------------------------------------------------------
// nsBBox2::PointInBox:
//---------------------------------------------------------
bool nsBBox2::PointInBox( float x, float y, float boxX, float boxY ) const
{
	nsVec2	chkMin( m_min.x + boxX, m_min.y + boxY );
	nsVec2	chkMax( m_max.x + boxX, m_max.y + boxY );
	return x >= chkMin.x && y >= chkMin.y && x <= chkMax.x && y <= chkMax.y;
}

//---------------------------------------------------------
// nsBBox2::PointInBox: 
//---------------------------------------------------------
bool nsBBox2::PointInBox( const nsVec2 &pt, const nsVec2 &boxPos ) const
{
	return PointInBox( pt.x, pt.y, boxPos.x, boxPos.y );
}

//---------------------------------------------------------
// nsBBox2::Intersect:
//---------------------------------------------------------
bool nsBBox2::Intersect( const nsBBox2 &box ) const
{
	float	distX = nsMath::Max( box.m_max.x, m_max.x ) - nsMath::Min( box.m_min.x, m_min.x );
	if ( distX < Width() + box.Width() )
	{
		float	distY = nsMath::Max( box.m_max.y, m_max.y ) - nsMath::Min( box.m_min.y, m_min.y );
		return distY < Height() + box.Height();
	}

	return false;
}

#endif