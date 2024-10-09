#include "Line2.h"
#include "StrTools.h"

//-----------------------------------------------------
// LineCross:
//-----------------------------------------------------
bool nsLine2::LineCross( const nsVec2 &from, const nsVec2 &to, nsVec2 &res )
{
	float	fromDist = norm.Dot( from ) + dist;
	float	toDist = norm.Dot( to ) + dist;
	if ( (fromDist < 0 && toDist < 0) ||
		(fromDist > 0 && toDist > 0 ) ) return false;

	nsVec2	dir = to - from;

	float	len = nsMath::Absf( fromDist ) + nsMath::Absf( toDist );
	res = to - dir * nsMath::Absf( toDist ) / len;

	return true;
}
