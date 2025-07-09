//
// Created by Roman on 6/3/2025.
//

#include "PacketsHandlingManager.h"
#include "nsLib/log.h"

void nsPacketsHandlingManager::SetHandler(int packetId, const HandlerCallback &handler) {
    _handlers[packetId] = handler;
}

void nsPacketsHandlingManager::RemoveHandler(int packetId) {
    _handlers.erase(packetId);
}

bool nsPacketsHandlingManager::HandlePacket(const nsPacket *packet) {
    const auto handler = _handlers.find(packet->id);
    if (handler != _handlers.end()) {
        handler->second(packet);
        return true;
    }
    return false;
}
