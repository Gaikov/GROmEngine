//
// Created by Roman on 8/18/2025.
//

#include "Random.h"
#include <ctime>

std::mt19937 nsRandom::rng;

void nsRandom::Seed() {
    rng.seed(static_cast<unsigned int>(std::time(nullptr)));
}

