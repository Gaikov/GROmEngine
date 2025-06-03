// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ClientConnection.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Networking/Net.h"
#include "Networking/common/BaseClientSocket.h"

class nsClientConnection;

class nsClientConnectionContext {
public:
    virtual ~nsClientConnectionContext() = default;
    virtual void ProcessPacket(nsClientConnection *from, nsPacket *packet) = 0;
    virtual void OnClientDisconnect(nsClientConnection *connection) = 0;
};

class nsClientConnection : public nsBaseClientSocket {
public:
    nsClientConnection(int socket, int id, nsClientConnectionContext *context);
    ~nsClientConnection() override;
    void Disconnect();
    int GetId() const { return _id; }

private:
    int         _id;

    std::thread _thread;

    void WaitThread();
};


