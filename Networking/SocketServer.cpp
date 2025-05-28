//
// Created by Roman on 5/28/2025.
//

#include "SocketServer.h"
#include "nsLib/log.h"
#include "Net.h"

nsServerSocket::nsServerSocket() : _socket(-1) {}

nsServerSocket::~nsServerSocket() {
    if (_socket >= 0) {
        closesocket(_socket);
    }
}

bool nsServerSocket::Listen(int port) {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    NET_ERROR("server socket", false)

    char opt = 1;
    setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    NET_ERROR("server setsockopt", false)

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(_socket, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0) {
        NET_ERROR("server bind", false)
    }

    if (listen(_socket, SOMAXCONN) < 0) {
        NET_ERROR("server listen", false)
    }

    Log::Info("Server listening: %i", port);
    return true;
}

int nsServerSocket::Accept() const {
    sockaddr_in client_addr = {};
    socklen_t client_len = sizeof(client_addr);

    Log::Info("Waiting for client...");
    int clientSocket = accept(_socket, (struct sockaddr *) &client_addr, &client_len);
    if (clientSocket < 0) {
        NET_ERROR("server accept", -1);
    }

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));

    Log::Info("Client connected: %s:%i", ip, ntohs(client_addr.sin_port));
    return clientSocket;
}
