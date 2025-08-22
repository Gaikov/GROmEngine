//
// Created by Roman on 5/29/2025.
//

#include "Networking/server/ClientConnection.h"
#include "nsLib/log.h"

nsClientConnection::nsClientConnection(const int socket, const int id, nsClientConnectionContext *context)
    : _clientId(id) {
    _socket = socket;

    _thread = std::thread([this, context]() {
        char buffer[MAX_PACKET_SIZE];
        const auto packet = reinterpret_cast<nsPacket *>(buffer);
        while (ReceivePacket(packet)) {
            context->ProcessPacket(this, packet);
        }
        _connected = false;
        context->OnClientDisconnect(this);
    });
}

nsClientConnection::~nsClientConnection() {
    WaitThread();
}

void nsClientConnection::Disconnect() {
    Log::Info("Disconnecting client forced: %i", _clientId);
    Close();
    WaitThread();
}

bool nsClientConnection::SendData(const void *data, unsigned int size) const {
    if (!_connected) {
        Log::Warning("Client already disconnected: %i", _clientId);
        return false;
    }
    return Send(data, size);
}

void nsClientConnection::WaitThread() {
    if (_thread.joinable()) {
        Log::Info("Waiting for client thread: %i", _clientId);
        _thread.join();
    }
}
