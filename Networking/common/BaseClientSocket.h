// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file BaseSocket.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Networking/Packet.h"

class nsBaseClientSocket {
public:
    bool ReceivePacket(nsPacket *packet) const;
    bool Receive(char* buffer, unsigned int maxSize) const;

private:
    int _socket = -1;

    int Recv(char* buffer, unsigned int maxSize) const;
};
