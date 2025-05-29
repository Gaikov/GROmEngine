// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SocketServer.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

class nsServerSocket {
public:
    nsServerSocket();
    virtual ~nsServerSocket();

    bool Listen(int port);

    int Accept() const;
    void Close();

private:
    int _socket;
};