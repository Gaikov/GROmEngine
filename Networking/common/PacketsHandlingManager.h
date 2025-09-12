// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file PacketsHandlingManager.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/headers.h"
#include "Networking/Packet.h"

class nsPacketsHandlingManager {
public:
    typedef std::function<void(const nsPacket*)>    HandlerCallback;
    typedef std::vector<HandlerCallback>            HandlersList;

    void AddHandler(uint16_t packetId, const HandlerCallback &handler);
    void ClearHandlers(uint16_t packetId);
    bool HandlePacket(const nsPacket* packet);

private:
    std::map<uint16_t, HandlersList> _handlers;
};
