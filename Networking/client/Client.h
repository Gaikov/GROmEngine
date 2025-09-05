// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Client.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Networking/serialization/BinaryState.h"
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

   typedef std::function<bool(const nsPacket*)>    PacketHandler;

   nsClient();
   virtual ~nsClient();
   void Connect(const char *ip, int port);
   void Disconnect();

   void SetPacketHandler(int packetId, const nsPacketsHandlingManager::HandlerCallback& handler);
   void ClearPacketHandler(int packetId);
   void SetCommonPacketsHandler(const PacketHandler& handler);
   void Update();

   template<typename TPacket>
   bool SendPacket(TPacket *packed) const {
      if (State == CONNECTED && State == _commitState) {
         return _socket.SendPacket(packed);
      }

      Log::Warning("Client is not connected");
      return false;
   }

   template<typename TPacket>
   bool SendState(TPacket *source, const nsBinaryState &state) {
      if (State == CONNECTED && State == _commitState) {
         nsPacketBuffer buff = {};
         auto p = reinterpret_cast<TPacket*>(&buff);
         *p = *source;
         const auto packet = reinterpret_cast<nsPacket*>(&buff);
         packet->id = TPacket::ID;
         packet->senderTime = nsTime::GetTimeMS();
         state.SerializePacket(p);
         return _socket.Send(p, p->size);
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
   std::vector<PacketHandler> _commonHandlers;
   std::string    _ip;

   void OnConnected();
   void OnPacketReceived(const nsPacket *packet);
   bool HandlePacket(const nsPacket *packet);
};

