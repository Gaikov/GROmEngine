// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Packet.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/headers.h"

#define MAX_PACKET_SIZE 128

struct nsPacketBuffer {
    char buffer[MAX_PACKET_SIZE];
};

enum nsTargetType {
    TARGET_ALL = 0,
    TARGET_SERVER = 1,
    TARGET_CLIENT = 2,
    TARGET_OTHER_CLIENTS = 3,
};

struct alignas(4) nsPacket {
    uint32_t id;
    uint32_t size;
    nsTargetType targetType;
    uint16_t targetId;

    template<typename TPacket>
    static void Init(TPacket *p) {
        auto *packet = reinterpret_cast<nsPacket*>(p);
        packet->id = TPacket::ID;
        packet->size = sizeof(TPacket);
    }
};

struct nsPacketId {
    static constexpr unsigned short CLIENT_ID = 0;
    static constexpr unsigned short MESSAGE = 1;
    static constexpr unsigned short LAST_ID = 2;
};

struct nsMessagePacket : nsPacket {
    static constexpr unsigned short ID = nsPacketId::MESSAGE;
    char message[96];
};

struct nsClientIdPacket : nsPacket {
    static constexpr unsigned short ID = nsPacketId::CLIENT_ID;
    uint16_t clientId;
};