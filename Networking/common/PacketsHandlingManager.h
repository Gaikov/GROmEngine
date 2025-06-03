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

    void SetHandler(int packetId, const HandlerCallback &handler);
    void RemoveHandler(int packetId);
    void HandlePacket(const nsPacket* packet);

private:
    std::map<int, HandlerCallback> _handlers;
};
