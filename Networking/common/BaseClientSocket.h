// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file BaseSocket.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/headers.h"
#include "Networking/Packet.h"

class nsBaseClientSocket {
public:
    virtual ~nsBaseClientSocket();

    bool ReceivePacket(nsPacket *packet) const;
    bool Receive(char* buffer, unsigned int maxSize) const;
    void Close();
    template<typename T>
    bool SendPacket(T *packed) const {
        const nsPacket *p = packed;
        nsPacket::Init(packed);
        return Send(p, p->size);
    }

    bool Send(const void *data, unsigned int size) const;

protected:
    int _socket = -1;

private:
    int Recv(char* buffer, unsigned int maxSize) const;

};
