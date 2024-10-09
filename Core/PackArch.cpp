//
// Created by Roman on 5/14/2024.
//
#include "PackArch.h"
#include "nsLib/log.h"

bool checkPackHeader(const packHeader_t &header) {
    if (strncmp(header.id, PACK_ID, 4) != 0) {
        Log::Warning("Invalid pack file header!");
        return false;
    }

    if (header.version != PACK_VERSION) {
        Log::Warning("Invalid pack file version: %i", header.version);
        return false;
    }

    return true;
}

