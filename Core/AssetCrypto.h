#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

#include "PackArch.h"

class nsAssetCrypto {
public:
    using Key = std::array<std::uint8_t, PACK_KEY_SIZE>;
    using Nonce = std::array<std::uint8_t, PACK_NONCE_SIZE>;
    using Tag = std::array<std::uint8_t, PACK_TAG_SIZE>;

    static bool Encrypt(void *data, std::size_t size, const Key &key,
                        const Nonce &nonce, const std::string &aad, Tag &tag);
    static bool Decrypt(void *data, std::size_t size, const Key &key,
                        const Nonce &nonce, const std::string &aad, const Tag &tag);
    static bool GenerateNonce(Nonce &nonce);
    static bool ParseHexKey(const std::string &value, Key &key);
    static bool LoadKey(const char *propertiesFile, Key &key);

    static std::string DirectoryAad(std::uint32_t filesCount);
    static std::string FileAad(const char *fileName, std::uint32_t size);
};
