// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Client.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Networking/client/ClientSocket.h"
#include "Networking/common/PacketsPool.h"

class nsClient {
public:
   enum State {
      DISCONNECTED,
      CONNECTING,
      CONNECTED,
   };

   nsClient();
   virtual ~nsClient();

   void Connect(const char *ip, int port);

private:
   nsClientSocket _socket;
   std::thread    _connectionThread;
   std::thread    _packetThread;
   State          _state = DISCONNECTED;
   nsPacketsPool  _packetsPool;
   std::mutex     _packetMutex;
   std::vector<nsPacket *> _receivedPackets;

   void OnConnected();
   void OnPacketReceived(const nsPacket *packet);
};
