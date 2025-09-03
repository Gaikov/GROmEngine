//
// Created by Roman on 9/3/2025.
//

#include "Time.h"

#include <chrono>

uint64_t nsTime::GetTimeMS() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch()
           ).count();
}
