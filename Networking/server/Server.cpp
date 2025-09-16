//
// Created by Roman on 5/29/2025.
//

#include "Server.h"

#include <iostream>

#include "ClientConnection.h"
#include "Networking/Packet.h"
#include "nsLib/log.h"

nsServer::nsServer(int port) : _port(port) {
    _timeoutClients.reserve(20);
    _disconnectedClients.reserve(20);
}

nsServer::~nsServer() {
    for (const auto c: _disconnectedClients) {
        delete c;
    }
    Log::Info("Server destroyed");
}

bool nsServer::Start() {
    if (!_socket.Listen(_port)) {
        return false;
    }

    _isRunning = true;
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
    return true;
}

void nsServer::Run(const int argc, char *argv[]) {
    _running = true;

    if (argc > 1 && strcmp(argv[1], "-daemon") == 0) {
        Log::Info("Server started in daemon mode");

        while (_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            DisconnectClientsWithTimout();
            ClearDisconnectedClients();
        }

    } else {
        Log::Info("Server started in interactive mode");

        std::thread t([this]() {
            while (_running) {
                std::string command;
                std::getline(std::cin, command);
                if (command == "quit") {
                    _running = false;
                    break;
                }
            }
        });

        while (_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            DisconnectClientsWithTimout();
            ClearDisconnectedClients();
        }

        t.join();
    }
}

void nsServer::Stop() {
    if (_isRunning) {
        Log::Info("Stopping server");

        _isRunning = false;
        _socket.Close();

        if (_clientsThread.joinable()) {
            _clientsThread.join();
        }

        std::vector<nsClientConnection *> connectedClients; {
            std::lock_guard lock(_clientsMutex);
            for (auto c: _clients) {
                connectedClients.push_back(c);
            }
        }

        for (auto c: connectedClients) {
            c->Disconnect();
        }

        Log::Info("Server stopped!");
    }
}

void nsServer::BroadcastPacket(const nsPacket *packet) const {
    for (auto c: _clients) {
        c->SendData(packet, packet->size);
    }
}

void nsServer::OnAcceptClient(int socket) {
    std::lock_guard lock(_clientsMutex);

    auto c = new nsClientConnection(socket, _clientLastId++, this);
    Log::Info("Client id assigned: %i", c->GetClientId());

    _clients.push_back(c);

    nsProtocolVersion version = {};
    nsPacket::Init(&version);
    version.version = GetProtocolVersion();
    if (!c->SendData(&version, version.size)) {
        Log::Error("Failed to send protocol version", c->GetClientId());
        return;
    }

    nsClientIdPacket p = {};
    p.clientId = c->GetClientId();
    nsPacket::Init(&p);

    if (!c->SendData(&p, p.size)) {
        Log::Error("Failed to send client id: ", c->GetClientId());
        return;
    }

    OnClientConnected(c);
}

void nsServer::ProcessPacket(nsClientConnection *from, nsPacket *packet) {
    std::lock_guard lock(_clientsMutex);
    SendPacket(from, packet);
}

void nsServer::SendPacket(const nsClientConnection *from, nsPacket *packet) {
    if (packet->targetType == TARGET_ALL) {
        for (const auto c: _clients) {
            c->SendData(packet, packet->size);
        }
    } else if (packet->targetType == TARGET_OTHER_CLIENTS) {
        for (const auto c: _clients) {
            if (c != from) {
                c->SendData(packet, packet->size);
            }
        }
    } else if (packet->targetType == TARGET_CLIENT) {
        const auto c = GetClient(packet->targetId);
        if (c) {
            c->SendData(packet, packet->size);
        }
    } else {
        Log::Info("packet from client: %i, packet: %i", from->GetClientId(), packet->id);
    }
}

nsClientConnection * nsServer::GetClient(uint32_t clientId) const {
    for (const auto c: _clients) {
        if (c->GetClientId() == clientId) {
            return c;
        }
    }
    Log::Warning("Client not found: %i", clientId);
    return nullptr;
}

void nsServer::DisconnectClientsWithTimout() {
    _timeoutClients.clear();
    {
        const auto currTime = nsTime::GetTimeMS();
        std::lock_guard lock(_clientsMutex);
        for (auto c : _clients) {
            if (currTime - c->GetLastActiveTime() > _clientTimout) {
                Log::Info("Client %i timeout, will be disconnected", c->GetClientId());
                _timeoutClients.push_back(c);
            }
        }
    }
    for (const auto client : _timeoutClients) {
        client->Disconnect();
    }
    _timeoutClients.clear();
}

void nsServer::ClearDisconnectedClients() {
    if (_disconnectedClients.size()) {
        Log::Info("Clearing disconnected clients: %i", _disconnectedClients.size());
    }

    for (const auto c : _disconnectedClients) {
        delete c;
    }
    _disconnectedClients.clear();
}

void nsServer::PerformClientDisconnect(nsClientConnection *c) {
    Log::Info("On Client disconnect: %i", c->GetClientId());

    auto it = std::find(_clients.begin(), _clients.end(), c);
    if (it == _clients.end()) {
        Log::Error("Client not found in list");
        return;
    }
    _clients.erase(it);

    Log::Info("Client disconnected: %i", c->GetClientId());
    _disconnectedClients.push_back(c);
}

void nsServer::OnClientDisconnect(nsClientConnection *c) {
    std::lock_guard lock(_clientsMutex);
    PerformClientDisconnect(c);
    OnClientDisconnected(c);
}
