// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ClientConnection.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

class nsClientConnection {

public:
    nsClientConnection(int socket, int id);
    ~nsClientConnection();
    bool Send(const void *data, int size) const;
    int Receive(void* buffer, int size) const;
    void Close();
    int GetId() const { return _id; }

private:
    int         _socket;
    int         _id;
};


