#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

// GLFW Cocoa's SDL-compatible GUID encoding. This is a routing fingerprint,
// not a connection identity: two equal controllers must remain independent.
inline std::array<char, 33> AppleGamepadGuid(uint32_t vendor, uint32_t product, uint32_t version, const char* name) {
    std::array<uint8_t, 16> bytes{};
    if (vendor && product) {
        bytes[0] = 3;
        bytes[4] = vendor;
        bytes[5] = vendor >> 8;
        bytes[8] = product;
        bytes[9] = product >> 8;
        bytes[12] = version;
        bytes[13] = version >> 8;
    } else {
        bytes[0] = 5;
        if (name) std::memcpy(bytes.data() + 4, name, std::min(size_t(11), std::strlen(name)));
    }
    constexpr auto digits = "0123456789abcdef";
    std::array<char, 33> guid{};
    for (auto i = size_t(0); i < bytes.size(); ++i) {
        guid[i * 2] = digits[bytes[i] >> 4];
        guid[i * 2 + 1] = digits[bytes[i] & 15];
    }
    return guid;
}

class nsAppleGamepadRouting {
public:
    enum class Route { Pending, Glfw, GameController };

    bool Contains(uint64_t identity) const {
        return std::any_of(_devices.begin(), _devices.end(), [=](const auto& device) { return device.identity == identity; });
    }
    void Connect(uint64_t identity, const char* guid, const char* name, bool supported, bool synthetic) {
        Disconnect(identity);
        _devices.push_back({identity, guid, name, supported || synthetic});
    }
    void Disconnect(uint64_t identity) {
        std::erase_if(_devices, [=](const auto& device) { return device.identity == identity; });
    }
    Route GetRoute(const char* guid, const char* name) const {
        auto covered = false, raw = false;
        for (const auto& device : _devices) {
            if (device.guid == guid && device.name == name) {
                covered |= device.covered;
                raw |= !device.covered;
            }
        }
        if (covered && !raw) return Route::GameController;
        if (raw && !covered) return Route::Glfw;
        return Route::Pending;
    }
    void Clear() { _devices.clear(); }

private:
    struct nsDevice {
        uint64_t identity;
        std::string guid;
        std::string name;
        bool covered;
    };
    std::vector<nsDevice> _devices;
};
