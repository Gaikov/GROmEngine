//
// Created by Roman on 5/8/2024.
//

#include <cstring>
#include "Crypt.h"

void nsCrypt::XorEncode(void *data, unsigned int length, const char *key) {
    auto keyLen = strlen((const char*)key);
    auto bytes = (unsigned char *)(data);
    auto keyBytes = (const unsigned char*)(key);

    for (int i = 0; i < length; i++) {
        bytes[i] = bytes[i] ^ keyBytes[i % keyLen];
    }
}