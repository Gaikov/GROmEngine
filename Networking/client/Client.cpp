//
// Created by Roman on 6/3/2025.
//

#include "Client.h"

nsClient::nsClient() {
    _packetsPool.Reserve(10);
    Log::Info("...client created");
}

nsClient::~nsClient() {
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
        _connectionThread = std::thread([this, ip, port]() {
            if (!_socket.Connect(ip, port)) {
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

void nsClient::OnConnected() {
    if (_packetThread.joinable()) {
        _packetThread.join();
    }

    _packetThread = std::thread([this] {
        nsPacketBuffer  buffer = {};
        const auto    packet = reinterpret_cast<nsPacket*>(&buffer);
        while (_socket.ReceivePacket(packet)) {
            Log::Info("client packet received: %i", packet->id);
            OnPacketReceived(packet);
        }

        _state = DISCONNECTED;
        Log::Info("client disconnected");
    });
}

void nsClient::OnPacketReceived(const nsPacket *packet) {
    std::lock_guard lock(_packetMutex);

    auto buffer = _packetsPool.AllocateObject();
    memcpy(buffer, packet, sizeof(nsPacketBuffer));
    _receivedPackets.push_back(reinterpret_cast<nsPacket*>(buffer));
}
