//
// Created by Roman on 5/29/2025.
//

#include "BaseClientSocket.h"
#include "Networking/Net.h"

nsBaseClientSocket::~nsBaseClientSocket() {
    Close();
}

bool nsBaseClientSocket::ReceivePacket(nsPacket *packet) const {

    auto headerSize = sizeof(nsPacket);
    const auto target = reinterpret_cast<char *>(packet);
    if (!Receive(target, headerSize)) {
        return false;
    }

    if (!Receive(target + headerSize, packet->size - headerSize)) {
        return false;
    }

    return true;
}

bool nsBaseClientSocket::Receive(char *buffer, unsigned int maxSize) const {
    int totalBytes = 0;
    while (totalBytes < maxSize) {
        auto numBytes = Recv(buffer + totalBytes, maxSize - totalBytes);
        if (numBytes <= 0) {
            return false;
        }
        totalBytes += numBytes;
    }
    return true;
}

int nsBaseClientSocket::Recv(char *buffer, unsigned int maxSize) const {
    const auto numBytes = recv(_socket, buffer, static_cast<int>(maxSize), 0);
    NET_ERROR("client recv", -1);
    return numBytes;
}

bool nsBaseClientSocket::Send(const void *data, unsigned int size) const {
    auto sent = send(_socket, static_cast<const char *>(data), static_cast<int>(size), 0);
    if (sent != size) {
        NET_ERROR("client send", false);
    }
    return true;
}

void nsBaseClientSocket::Close() const {
    if (_socket >= 0) {
        closesocket(_socket);
        NET_ERROR("close client socket",);
    }
}

