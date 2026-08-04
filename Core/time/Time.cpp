//
// Created by Roman on 9/3/2025.
//

#include "Time.h"

#include <chrono>

uint64_t nsTime::GetTimeMS() {
    return GetTimeUS() / 1000;
}

uint64_t nsTime::GetTimeUS() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::steady_clock::now().time_since_epoch()
           ).count();
}
