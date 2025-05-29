//
// Created by Roman on 5/29/2025.
//

#include "Server.h"

#include "ClientConnection.h"
#include "Networking/Packet.h"
#include "nsLib/log.h"

nsServer::nsServer(int port) : _port(port) {
}

nsServer::~nsServer() {
    Stop();
    Log::Info("Server stopped!");
}

bool nsServer::Start() {
    if (!_socket.Listen(_port)) {
        return false;
    }

    _clientsThread = std::thread([this]() {
        OnAcceptClient(_socket.Accept());
    });
    Log::Info("Server started");
    _isRunning = true;
    return true;
}

void nsServer::Stop() {
    if (_isRunning) {
        Log::Info("Stopping server");

        _isRunning = false;
        _socket.Close();
        _clientsThread.join();
    }
}

void nsServer::OnAcceptClient(int socket) {
    std::lock_guard lock(_clientsMutex);

    auto c = new nsClientConnection(socket, _clientLastId++);
    Log::Info("Client id assigned: %i", c->GetId());

    _clients.push_back(c);

    nsMessagePacket p = {};
    p.id = nsPackageId::MESSAGE;
    p.owner = -1;
    p.size = sizeof(p);
    strcpy(p.message,  "Hello from server!");

    if (!c->Send(&p, sizeof(p))) {
        Log::Error("Failed to send hello to client", c->GetId());
    }

    std::thread([this, c]() {
        c->Receive()
    });
}

