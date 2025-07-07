#pragma once
#include "BinaryVar.h"

class nsBinaryState final {
public:
    void RegisterVar(IBinaryVar *var);
    [[nodiscard]] size_t GetBufferSize() const { return _bufferSize; }
    void Serialize(void *buffer) const;
    void Deserialize(const void *buffer) const;

private:
    IBinaryVar  *_vars = nullptr;
    size_t      _bufferSize = 0;
};
