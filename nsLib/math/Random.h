// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Random.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <random>

struct nsRandom {
    static void Seed();
    static float Random();
    static int RandomInt( int min, int max );

private:
    static std::mt19937 rng;
};

inline float nsRandom::Random() {
    std::uniform_real_distribution<float> dist(0.0, 1.0);
    return dist(rng);
}

inline int nsRandom::RandomInt(int min, int max) {
    std::uniform_int_distribution dist(min, max);
    return dist(rng);
}

