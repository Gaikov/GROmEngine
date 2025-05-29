// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Packet.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

enum nsTargetType {
    ALL = 0,
    SERVER = 1,
    CLIENT = 2,
};

struct nsPacket {
    unsigned short id;
    unsigned int size;
    unsigned short owner;
    unsigned char targetType;
    unsigned short targetClient;
};

enum nsPackageId {
    MESSAGE = 0,
};

struct nsMessagePacket : nsPacket {
    char message[64];
};