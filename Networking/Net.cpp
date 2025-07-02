//
// Created by Roman on 5/28/2025.
//

#include "Net.h"
#include "nsLib/log.h"

bool nsNet::CheckLastError(const char* message) {
    if (const auto code = GetLastError()) {
        Log::Error("%s: %i - %s", message, code, GetErrorMessage(code));
        return false;
    }
    return true;
}
