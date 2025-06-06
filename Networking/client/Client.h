// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Client.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Networking/client/ClientSocket.h"
#include "Networking/common/PacketsHandlingManager.h"
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

   State GetState() const { return _state; }
   void Connect(const char *ip, int port);
   void Disconnect();

   void AddPacketHandler(int packetId, const nsPacketsHandlingManager::HandlerCallback& handler);
   void ProcessPackets();

   template<typename T>
   bool SendPacket(T *packed) const {
      if (_state != CONNECTED) {
         Log::Warning("Client is not connected");
         return false;
      }
      return _socket.SendPacket(packed);
   }

private:
   nsClientSocket _socket;
   std::thread    _connectionThread;
   std::thread    _packetThread;
   State          _state = DISCONNECTED;
   nsPacketsPool  _packetsPool;
   std::mutex     _packetMutex;
   std::vector<nsPacketBuffer *> _receivedPackets;
   nsPacketsHandlingManager _packetsHandling;
   std::string _ip;

   void OnConnected();
   void OnPacketReceived(const nsPacket *packet);
};
