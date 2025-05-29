// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Packet.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#define MAX_PACKET_SIZE 128

enum nsTargetType {
    ALL = 0,
    SERVER = 1,
    CLIENT = 2,
};

struct alignas(1) nsPacketHeader {
    unsigned short id;
    unsigned int size;
};

struct nsPacket : nsPacketHeader {
    unsigned short owner;       //server -1
    unsigned char targetType;
    unsigned char targetClient;
};

struct nsPackageId {
    static constexpr unsigned short MESSAGE = 0;
};

struct nsMessagePacket : nsPacket {
    char message[96];
};