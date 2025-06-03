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
    for (const auto c : _disconnectedClients) {
        delete c;
    }
    Log::Info("Server destroyed");
}

bool nsServer::Start() {
    if (!_socket.Listen(_port)) {
        return false;
    }

    _clientsThread = std::thread([this]() {
        while (_isRunning) {
            auto socket = _socket.Accept();
            if (socket >= 0) {
                OnAcceptClient(socket);
            }
        }
        Log::Info("Server thread stopping");
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

        if (_clientsThread.joinable()) {
            _clientsThread.join();
        }

        std::vector<nsClientConnection *> connectedClients;
        {
            std::lock_guard lock(_clientsMutex);
            for (auto c : _clients) {
                connectedClients.push_back(c);
            }
        }

        for (auto c : connectedClients) {
            c->Disconnect();
        }

        Log::Info("Server stopped!");
    }
}

void nsServer::OnAcceptClient(int socket) {
    std::lock_guard lock(_clientsMutex);

    auto c = new nsClientConnection(socket, _clientLastId++, this);
    Log::Info("Client id assigned: %i", c->GetId());

    _clients.push_back(c);

    nsClientIdPacket p = {};
    p.id = nsPacketId::CLIENT_ID;
    p.clientId = c->GetId();
    p.size = sizeof(p);

    if (!c->Send(&p, p.size)) {
        Log::Error("Failed to send client id: ", c->GetId());
    }
}

void nsServer::ProcessPacket(nsClientConnection *from, nsPacket *packet) {
    Log::Info("packet from client (%i): %i", from->GetId(), packet->id);
}

void nsServer::OnClientDisconnect(nsClientConnection *c) {
    Log::Info("On Client disconnect: %i", c->GetId());
    std::lock_guard lock(_clientsMutex);
    auto it = std::find(_clients.begin(), _clients.end(), c);
    if (it == _clients.end()) {
        Log::Error("Client not found in list");
        return;
    }
    _clients.erase(it);

    Log::Info("Client disconnected: %i", c->GetId());
    _disconnectedClients.push_back(c);
}
