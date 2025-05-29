// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file nsServerApp.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Networking/Net.h"
#include "SocketServer.h"

class nsServer {
public:
    explicit nsServer(int port);
    virtual ~nsServer();
    bool Start();
    void Stop();

private:
    int             _port;
    nsServerSocket  _socket;
    std::thread     _clientsThread;
    bool            _isRunning = false;
};
