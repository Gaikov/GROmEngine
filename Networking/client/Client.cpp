//
// Created by Roman on 6/3/2025.
//

#include "Client.h"

nsClient::nsClient() {
    _packetsPool.Reserve(10);
    Log::Info("...client created");
}

nsClient::~nsClient() {
    Disconnect();
    if (_packetThread.joinable()) {
        _packetThread.join();
    }

    if (_connectionThread.joinable()) {
        _connectionThread.join();
    }
    Log::Info("...client destroyed");
}

void nsClient::Connect(const char *ip, int port) {
    if (_state == DISCONNECTED) {
        _ip = ip;
        if (!_socket.Open()) {
            return;
        }
        _connectionThread = std::thread([this, port]() {
            _state = CONNECTING;
            if (!_socket.Connect(_ip.c_str(), port)) {
                _state = DISCONNECTED;
            } else {
                _state = CONNECTED;
                OnConnected();
            }
        });
    } else {
        Log::Error("client is already connected");
    }
}

void nsClient::Disconnect() {
    _socket.Close();
    if (_connectionThread.joinable()) {
        _connectionThread.join();
    }
    _state = DISCONNECTED;

    std::lock_guard lock(_packetMutex);
    for (auto p : _receivedPackets) {
        _packetsPool.RecycleObject(p);
    }
    _receivedPackets.clear();
}

void nsClient::AddPacketHandler(const int packetId, const nsPacketsHandlingManager::HandlerCallback &handler) {
    _packetsHandling.SetHandler(packetId, handler);
}

void nsClient::ProcessPackets() {
    std::lock_guard lock(_packetMutex);
    for (auto p : _receivedPackets) {
        _packetsHandling.HandlePacket(reinterpret_cast<nsPacket*>(p));
        _packetsPool.RecycleObject(p);
    }
    _receivedPackets.clear();
}

void nsClient::OnConnected() {
    if (_packetThread.joinable()) {
        _packetThread.join();
    }

    _packetThread = std::thread([this] {
        nsPacketBuffer  buffer = {};
        const auto    packet = reinterpret_cast<nsPacket*>(&buffer);
        while (_socket.ReceivePacket(packet)) {
            OnPacketReceived(packet);
        }

        _state = DISCONNECTED;
        Log::Info("client disconnected");
    });
}

void nsClient::OnPacketReceived(const nsPacket *packet) {
    std::lock_guard lock(_packetMutex);

    auto buffer = _packetsPool.AllocateObject();
    memcpy(buffer, packet, packet->size);
    _receivedPackets.push_back(buffer);
}
