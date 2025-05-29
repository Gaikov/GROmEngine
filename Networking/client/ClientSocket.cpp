//
// Created by Roman on 5/29/2025.
//

#include "ClientSocket.h"
#include "Networking/Net.h"
#include "nsLib/log.h"

nsClientSocket::nsClientSocket() : _socket(-1) {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) {
        NET_ERROR("client socket",);
    }
}

nsClientSocket::~nsClientSocket() {
    Close();
}

bool nsClientSocket::Connect(const char *ip, int port) const {
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serverAddr.sin_addr) <= 0) {
        NET_ERROR("client inet_pton", false);
    }

    if (connect(_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        NET_ERROR("client connect", false);
    }

    Log::Info("Connected to: %s:%i", ip, port);
    return true;
}

bool nsClientSocket::Send(const void *data, const int size) const {
    int sent = send(_socket, static_cast<const char *>(data), size, 0);
    if (sent != size) {
        NET_ERROR("client send", false);
        return false;
    }
    return true;
}

int nsClientSocket::Receive(char* buffer, const int size) const {
    const auto numBytes = recv(_socket, buffer, size, 0);
    NET_ERROR("client recv", -1);
    return numBytes;
}

void nsClientSocket::Close() {
    if (_socket >= 0) {
        closesocket(_socket);
        _socket = -1;
        Log::Info("Client socked closed");
    }
}
