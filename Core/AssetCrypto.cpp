#include "AssetCrypto.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <random>

#include <mbedtls/gcm.h>

namespace {

int HexDigit(char value) {
    if (value >= '0' && value <= '9') return value - '0';
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    return -1;
}

bool Configure(mbedtls_gcm_context &context, const nsAssetCrypto::Key &key) {
    mbedtls_gcm_init(&context);
    return mbedtls_gcm_setkey(&context, MBEDTLS_CIPHER_ID_AES,
                              key.data(), key.size() * 8) == 0;
}

} // namespace

bool nsAssetCrypto::Encrypt(void *data, std::size_t size, const Key &key,
                            const Nonce &nonce, const std::string &aad, Tag &tag) {
    mbedtls_gcm_context context;
    if (!Configure(context, key)) return false;

    auto *bytes = static_cast<unsigned char *>(data);
    const auto result = mbedtls_gcm_crypt_and_tag(
            &context, MBEDTLS_GCM_ENCRYPT, size, nonce.data(), nonce.size(),
            reinterpret_cast<const unsigned char *>(aad.data()), aad.size(),
            bytes, bytes, tag.size(), tag.data());
    mbedtls_gcm_free(&context);
    return result == 0;
}

bool nsAssetCrypto::Decrypt(void *data, std::size_t size, const Key &key,
                            const Nonce &nonce, const std::string &aad, const Tag &tag) {
    mbedtls_gcm_context context;
    if (!Configure(context, key)) return false;

    auto *bytes = static_cast<unsigned char *>(data);
    const auto result = mbedtls_gcm_auth_decrypt(
            &context, size, nonce.data(), nonce.size(),
            reinterpret_cast<const unsigned char *>(aad.data()), aad.size(),
            tag.data(), tag.size(), bytes, bytes);
    mbedtls_gcm_free(&context);
    return result == 0;
}

bool nsAssetCrypto::GenerateNonce(Nonce &nonce) {
    std::random_device random;
    for (auto &value : nonce) value = static_cast<std::uint8_t>(random());
    return true;
}

bool nsAssetCrypto::ParseHexKey(const std::string &value, Key &key) {
    if (value.size() != key.size() * 2) return false;
    for (std::size_t i = 0; i < key.size(); ++i) {
        const auto hi = HexDigit(value[i * 2]);
        const auto lo = HexDigit(value[i * 2 + 1]);
        if (hi < 0 || lo < 0) return false;
        key[i] = static_cast<std::uint8_t>((hi << 4) | lo);
    }
    return true;
}

bool nsAssetCrypto::LoadKey(const char *propertiesFile, Key &key) {
    std::ifstream input(propertiesFile);
    if (!input) return false;

    std::string line;
    constexpr auto prefix = "assetEncryptionKey=";
    while (std::getline(input, line)) {
        if (line.rfind(prefix, 0) == 0) {
            return ParseHexKey(line.substr(std::char_traits<char>::length(prefix)), key);
        }
    }
    return false;
}

std::string nsAssetCrypto::DirectoryAad(std::uint32_t filesCount) {
    return "PACK2:DIRECTORY:" + std::to_string(filesCount);
}

std::string nsAssetCrypto::FileAad(const char *fileName, std::uint32_t size) {
    return "PACK2:FILE:" + std::string(fileName) + ":" + std::to_string(size);
}
