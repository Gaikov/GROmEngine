//
// Created by Roman on 5/29/2025.
//

#include "Networking/server/ClientConnection.h"
#include "nsLib/log.h"

nsClientConnection::nsClientConnection(const int socket, const int id, nsClientConnectionContext *context)
    : _id(id) {
    _socket = socket;

    _thread = std::thread([this, context]() {
        char buffer[MAX_PACKET_SIZE];
        const auto packet = reinterpret_cast<nsPacket *>(buffer);
        while (ReceivePacket(packet)) {
            context->ProcessPacket(this, packet);
        }
        context->OnClientDisconnect(this);
    });
}

nsClientConnection::~nsClientConnection() {
    _thread.detach();
}

void nsClientConnection::Disconnect() {
    Log::Info("Disconnecting client forced: %i", _id);
    Close();
    if (_thread.joinable()) {
        _thread.join();
    }
}
