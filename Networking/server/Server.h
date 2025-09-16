// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file nsServerApp.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Networking/server/ClientConnection.h"
#include "Networking/server/SocketServer.h"
#include "Networking/Packet.h"

class nsServer : public nsClientConnectionContext {
public:
    explicit nsServer(int port);
    ~nsServer() override;

    bool Start();
    void Run(int argc, char *argv[]);
    void Stop();

protected:
    std::mutex      _clientsMutex;
    std::vector<nsClientConnection*> _clients;
    int32_t _clientTimout = 1000 * 30;

    void BroadcastPacket(const nsPacket *packet) const;
    virtual void SendPacket(const nsClientConnection *from, nsPacket *packet);

    virtual void OnClientConnected(nsClientConnection *c) {}
    virtual void OnClientDisconnected(const nsClientConnection *c) {}
    nsClientConnection* GetClient(uint32_t clientId) const;
    virtual uint32_t GetProtocolVersion() const = 0;

private:

    std::atomic<bool> _running = false;
    int             _port;
    nsServerSocket  _socket;
    std::thread     _clientsThread;
    bool            _isRunning = false;
    int             _clientLastId = 0;
    std::vector<nsClientConnection*> _disconnectedClients;
    std::vector<nsClientConnection*> _timeoutClients;

    void DisconnectClientsWithTimout();
    void ClearDisconnectedClients();

    void OnAcceptClient(int socket);
    void ProcessPacket(nsClientConnection *from, nsPacket *packet) override;
    void OnClientDisconnect(nsClientConnection *c) override;
    void PerformClientDisconnect(nsClientConnection *c);
};
