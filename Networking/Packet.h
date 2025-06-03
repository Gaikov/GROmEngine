// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Packet.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#define MAX_PACKET_SIZE 128

struct nsPacketBuffer {
    char buffer[MAX_PACKET_SIZE];
};

enum nsTargetType {
    ALL = 0,
    SERVER = 1,
    CLIENT = 2,
};

struct alignas(1) nsPacket {
    unsigned short id;
    unsigned int size;
    unsigned short owner;       //server -1
    unsigned char targetType;
    unsigned char targetClient;
};

struct nsPacketId {
    static constexpr unsigned short CLIENT_ID = 0;
    static constexpr unsigned short MESSAGE = 1;
};

struct nsMessagePacket : nsPacket {
    char message[96];
};

struct nsClientIdPacket : nsPacket {
    int clientId;
};