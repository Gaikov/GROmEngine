#pragma once
#include "BinaryVar.h"
#include "Networking/Packet.h"

class nsBinaryState final {
public:
    void RegisterVar(IBinaryVar *var);
    [[nodiscard]] size_t GetBufferSize() const { return _bufferSize; }
    void Serialize(void *buffer) const;
    void Deserialize(const void *buffer) const;

    template<typename TPacket>
    void SerializePacket(TPacket *packet) const {
        nsPacket *p = packet;

        p->id = TPacket::ID;
        p->size = GetBufferSize() + sizeof(TPacket);
        assert(p->size < MAX_PACKET_SIZE);

        char *buffer = reinterpret_cast<char*>(packet);
        Serialize(buffer + sizeof(TPacket));
    }

    template<typename TPacket>
    void DeserializePacket(const TPacket *packet) const {
        const nsPacket *p = packet;
        assert(p->size - sizeof(TPacket) == GetBufferSize());

        auto buffer = reinterpret_cast<const char*>(packet);
        Deserialize(buffer + sizeof(TPacket));
    }

private:
    IBinaryVar *_vars = nullptr;
    size_t      _varCount = 0;
    size_t      _bufferSize = 0;
};
