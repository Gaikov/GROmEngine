//
// Created by Roman on 9/25/2024.
//

#include "LineMath.h"


float nsLineMath::PointOnLine( const nsVec2 &in, nsVec2 &out, const nsVec2 &p1, const nsVec2 &p2 )
{
    out = p2 - p1;
    float 	lineLen = out.Length();
    out /= lineLen;

    float	dist = out.Dot( in - p1 );
    out = p1 + out * dist;

    if ( dist < 0 )
        return -dist;
    else if ( dist > lineLen )
        return dist - lineLen;

    float	toP2 = lineLen - dist;
    return -(toP2 < dist ? toP2 : dist);
}