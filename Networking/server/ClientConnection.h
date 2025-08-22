// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ClientConnection.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Networking/common/BaseClientSocket.h"

class nsClientConnection;

class nsClientConnectionContext {
public:
    virtual ~nsClientConnectionContext() = default;
    virtual void ProcessPacket(nsClientConnection *from, nsPacket *packet) = 0;
    virtual void OnClientDisconnect(nsClientConnection *connection) = 0;
};

class nsClientConnection : protected nsBaseClientSocket {
public:
    nsClientConnection(int socket, int id, nsClientConnectionContext *context);
    ~nsClientConnection() override;
    void Disconnect();
    [[nodiscard]] int32_t GetClientId() const { return _clientId; }

    bool SendData(const void *data, unsigned int size) const;


private:
    int32_t         _clientId;

    std::thread _thread;
    std::atomic<bool> _connected = true;

    void WaitThread();
};


