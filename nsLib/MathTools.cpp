#include "MathTools.h"

bool nsMath::IsValidTriangle( int i1, int i2, int i3 )
{
	return i1 != i2 && i1 != i3 && i2 != i3;
}

bool nsMath::InRange(float a, float min, float max)
{
	if (min > max)
	{
		std::swap(min, max);
	}

	return min <= a && a <= max;
}

float nsMath::Move(float from, float to, float delta) {
    float dir = to - from;
    if (dir == 0.0f) {
        return to;
    }

    dir /= fabs(dir);

    auto prev = from;
    auto curr = from + dir * delta;
    if (InRange(to, prev, from)) {
        curr = to;
    }

    return curr;
}

float nsMath::MoveExp(float from, float to, float speedScale, float deltaTime) {
    float speed = fabsf(to - from) * speedScale;
    return Move(from, to, speed * deltaTime);
}

