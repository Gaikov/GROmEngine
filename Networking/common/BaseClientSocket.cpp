//
// Created by Roman on 5/29/2025.
//

#include "BaseClientSocket.h"
#include "Networking/Net.h"

bool nsBaseClientSocket::ReceivePacket(nsPacket *packet) const {

    auto headerSize = sizeof(nsPacketHeader);
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
