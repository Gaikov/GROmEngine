#include "Vec2.h"

const nsVec2 nsVec2::null = nsVec2( 0, 0 );

nsVec2 nsVec2::Min(const nsVec2 &v1, const nsVec2 &v2) {
    return nsVec2( std::min(v1.x, v2.x), std::min(v1.y, v2.y) );
}

nsVec2 nsVec2::Max(const nsVec2 &v1, const nsVec2 &v2) {
    return nsVec2( std::max(v1.x, v2.x), std::max(v1.y, v2.y) );
}