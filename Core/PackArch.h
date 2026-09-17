// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file PackArch.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <cstring>

#define	PACK_VERSION	0x0200
#define PACK_ID "PACK"
#define PACK_MAX_PATH   128
#define PACK_KEY_SIZE 32
#define PACK_NONCE_SIZE 12
#define PACK_TAG_SIZE 16

#pragma pack( push, 1 )

struct packHeader_t {
    char id[4];
    std::uint16_t version;
    std::uint16_t reserved;
    std::uint32_t filesCount;
    std::uint32_t dirSize;
    std::uint8_t dirNonce[PACK_NONCE_SIZE];
    std::uint8_t dirTag[PACK_TAG_SIZE];
};

struct packFileDesc_t {
    char filename[PACK_MAX_PATH];
    std::uint64_t offset;
    std::uint32_t size;
    std::uint8_t nonce[PACK_NONCE_SIZE];
    std::uint8_t tag[PACK_TAG_SIZE];
};

#pragma pack( pop )

bool checkPackHeader(const packHeader_t &header);
