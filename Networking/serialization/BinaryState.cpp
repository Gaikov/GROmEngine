//
// Created by roman on 7/7/25.
//

#include "BinaryState.h"

void nsBinaryState::RegisterVar(IBinaryVar *var) {
   assert(var->next == nullptr);
   var->next = _vars;
   _vars = var;
   _bufferSize += var->GetSize();
}

void nsBinaryState::Serialize(void *buffer) const {
   auto data = static_cast<int8_t *>(buffer);
   for (auto var = _vars; var; var = var->next) {
      var->Serialize(data);
      data += var->GetSize();
   }
}

void nsBinaryState::Deserialize(const void *buffer) const {
   auto data = static_cast<const int8_t *>(buffer);
   for (auto var = _vars; var; var = var->next) {
      var->Deserialize(data);
      data += var->GetSize();
   }
}
