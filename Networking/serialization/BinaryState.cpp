//
// Created by roman on 7/7/25.
//

#include "BinaryState.h"

void nsBinaryState::RegisterVar(IBinaryVar *var) {
   assert(_varCount < MAX_VARS);
   _vars[_varCount++] = var;
   _bufferSize += var->GetSize();
}

void nsBinaryState::Serialize(void *buffer) const {
   auto data = static_cast<int8_t *>(buffer);
   for (int i = 0; i < _varCount; i++) {
      const auto var = _vars[i];
      var->Serialize(data);
      data += var->GetSize();
   }
}

void nsBinaryState::Deserialize(const void *buffer) const {
   auto data = static_cast<const int8_t *>(buffer);
   for (int i = 0; i < _varCount; i++) {
      const auto var = _vars[i];
      var->Deserialize(data);
      data += var->GetSize();
   }
}
