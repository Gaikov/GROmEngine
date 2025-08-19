//
// Created by Roman on 8/18/2025.
//

#include "Random.h"

std::mt19937 nsRandom::rng;

void nsRandom::Seed() {
    rng.seed(std::random_device{}());
}

