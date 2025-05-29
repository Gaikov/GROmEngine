// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ClientSocket.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once


class nsClientSocket {
public:
    nsClientSocket();
    ~nsClientSocket();

    bool Connect(const char *ip, int port) const;
    bool Send(const void *data, int size) const;
    int Receive(char* buffer, int size) const;
    void Close();

private:
    int _socket;
};