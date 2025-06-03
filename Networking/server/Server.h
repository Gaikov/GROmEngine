// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file nsServerApp.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once


#include "Networking/Net.h"
#include "Networking/server/ClientConnection.h"

#include "Networking/server/SocketServer.h"
#include "Networking/Packet.h"

class nsServer : public nsClientConnectionContext {
public:
    explicit nsServer(int port);
    ~nsServer() override;

    bool Start();
    void Stop();

private:
    int             _port;
    nsServerSocket  _socket;
    std::thread     _clientsThread;
    bool            _isRunning = false;
    int             _clientLastId = 0;
    std::mutex      _clientsMutex;
    std::vector<nsClientConnection*> _clients;

private:
    void OnAcceptClient(int socket);

public:
    void ProcessPacket(nsClientConnection *from, nsPacket *packet) override;
    void OnClientDisconnect(nsClientConnection *c) override;
};
