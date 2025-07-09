// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Client.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Networking/client/ClientSocket.h"
#include "Networking/common/PacketsHandlingManager.h"
#include "Networking/common/PacketsPool.h"
#include "nsLib/models/Property.h"

class nsClient {
public:
   enum ConnectionState {
      DISCONNECTED,
      CONNECTING,
      CONNECTED,
   };

   nsProperty<ConnectionState> State = DISCONNECTED;

   nsClient();
   virtual ~nsClient();
   void Connect(const char *ip, int port);
   void Disconnect();

   void AddPacketHandler(int packetId, const nsPacketsHandlingManager::HandlerCallback& handler);
   void AddCommonPacketsHandler(const nsPacketsHandlingManager::HandlerCallback& handler);
   void Update();

   template<typename T>
   bool SendPacket(T *packed) const {
      if (State == CONNECTED && State == _commitState) {
         return _socket.SendPacket(packed);
      }

      Log::Warning("Client is not connected");
      return false;
   }

private:
   nsClientSocket _socket;
   std::thread    _connectionThread;
   std::thread    _packetThread;
   ConnectionState _commitState = DISCONNECTED;
   nsPacketsPool  _packetsPool;
   std::mutex     _packetMutex;
   std::vector<nsPacketBuffer *> _receivedPackets;
   nsPacketsHandlingManager _packetsHandling;
   std::vector<nsPacketsHandlingManager::HandlerCallback> _commonHandlers;
   std::string    _ip;

   void OnConnected();
   void OnPacketReceived(const nsPacket *packet);
};
