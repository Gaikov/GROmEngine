//
// Created by Roman on 5/29/2025.
//

#include "ClientConnection.h"
#include "Networking/Net.h"

nsClientConnection::nsClientConnection(int socket, int id)
    : _socket(socket), _id(id) {

}

nsClientConnection::~nsClientConnection() {
    Close();
}

bool nsClientConnection::Send(const void *data, int size) const {
    const auto sent = send(_socket, static_cast<const char *>(data), size, 0);
    if (sent != size) {
        NET_ERROR("client send", false);
    }
    return true;
}

int nsClientConnection::Receive(void* buffer, int size) const {
    auto numBytes = recv(_socket, static_cast<char *>(buffer), size, 0);
    NET_ERROR("client recv", -1);
    return numBytes;
}

void nsClientConnection::Close() {
    if (_socket >= 0) {
        closesocket(_socket);
        _socket = -1;
    }
}
