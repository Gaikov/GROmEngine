//
// Created by Roman on 6/3/2025.
//

#include "PacketsHandlingManager.h"
#include "nsLib/log.h"

void nsPacketsHandlingManager::AddHandler(const uint16_t packetId, const HandlerCallback &handler) {
    _handlers[packetId].push_back(handler);
}

void nsPacketsHandlingManager::ClearHandlers(const uint16_t packetId) {
    _handlers.erase(packetId);
}

bool nsPacketsHandlingManager::HandlePacket(const nsPacket *packet) {
    const auto handler = _handlers.find(packet->id);
    if (handler != _handlers.end()) {
        for (const auto &h : handler->second) {
            h(packet);
        }
        return true;
    }
    return false;
}
