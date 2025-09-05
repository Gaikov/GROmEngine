// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Packet.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Core/time/Time.h"
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
    TARGET_MASTER = 4,
};

struct alignas(8) nsPacket {
    uint32_t id;
    uint32_t size;
    nsTargetType targetType;
    int32_t targetId;
    uint64_t senderTime;

    template<typename TPacket>
    static void Init(TPacket *p) {
        auto *packet = reinterpret_cast<nsPacket*>(p);
        packet->id = TPacket::ID;
        packet->size = sizeof(TPacket);
        packet->senderTime = nsTime::GetTimeMS();
        assert(p->size <= MAX_PACKET_SIZE);
    }

    float GetDeltaTime() const {
        const auto currTime = nsTime::GetTimeMS();
        assert (currTime > senderTime);
        return static_cast<float>(nsTime::GetTimeMS() - senderTime) / 1000.0f;
    }
};

struct nsPacketId {
    static constexpr unsigned short CLIENT_ID = 0;
    static constexpr unsigned short MESSAGE = 1;
    static constexpr unsigned short PROTOCOL_VERSION = 2;

    static constexpr unsigned short LAST_ID = 3;
};

struct nsMessagePacket : nsPacket {
    static constexpr unsigned short ID = nsPacketId::MESSAGE;
    enum {
        MAX_MESSAGE_SIZE = 96,
    };
    char message[MAX_MESSAGE_SIZE + 1];
};

struct nsClientIdPacket : nsPacket {
    static constexpr unsigned short ID = nsPacketId::CLIENT_ID;
    int32_t clientId;
};

struct nsProtocolVersion : nsPacket {
    static constexpr unsigned short ID = nsPacketId::PROTOCOL_VERSION;
    uint32_t version;
};