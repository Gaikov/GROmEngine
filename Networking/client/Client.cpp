//
// Created by Roman on 6/3/2025.
//

#include "Client.h"

nsClient::nsClient() {
    _packetsPool.Reserve(10);
    _receivedPackets.reserve(10);
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
    if (State == DISCONNECTED) {
        _ip = ip;
        if (!_socket.Open()) {
            return;
        }
        _connectionThread = std::thread([this, port]() {
            _commitState = CONNECTING;
            if (!_socket.Connect(_ip.c_str(), port)) {
                _commitState = DISCONNECTED;
            } else {
                _commitState = CONNECTED;
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
    State = _commitState = DISCONNECTED;

    std::lock_guard lock(_packetMutex);
    for (auto p : _receivedPackets) {
        _packetsPool.RecycleObject(p);
    }
    _receivedPackets.clear();
}

void nsClient::AddPacketHandler(const int packetId, const nsPacketsHandlingManager::HandlerCallback &handler) {
    _packetsHandling.SetHandler(packetId, handler);
}

void nsClient::AddCommonPacketsHandler(const PacketHandler &handler) {
    _commonHandlers.push_back(handler);
}

void nsClient::Update() {
    std::lock_guard lock(_packetMutex);

    State = _commitState;
    for (const auto p : _receivedPackets) {
        const auto packet = reinterpret_cast<nsPacket*>(p);
        if (!HandlePacket(packet)) {
            Log::Warning("net packet not handled: %i", packet->id);
        }
        _packetsPool.RecycleObject(p);
    }
    _receivedPackets.clear();
}

bool nsClient::HandlePacket(const nsPacket *packet) {
    bool handled = _packetsHandling.HandlePacket(packet);
    for (auto &h : _commonHandlers) {
        if (h(packet)) {
            handled = true;
        }
    }
    return handled;
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

        _commitState = DISCONNECTED;
        Log::Info("client disconnected");
    });
}

void nsClient::OnPacketReceived(const nsPacket *packet) {
    std::lock_guard lock(_packetMutex);

    auto buffer = _packetsPool.AllocateObject();
    memcpy(buffer, packet, packet->size);
    _receivedPackets.push_back(buffer);
}
