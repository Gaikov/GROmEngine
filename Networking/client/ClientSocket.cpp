//
// Created by Roman on 5/29/2025.
//

#include "ClientSocket.h"
#include "Networking/Net.h"
#include "nsLib/log.h"

nsClientSocket::nsClientSocket() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) {
        NET_ERROR("client socket",);
    }
}

bool nsClientSocket::Connect(const char *ip, int port) const {
    Log::Info("Connecting to: %s:%i", ip, port);
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


