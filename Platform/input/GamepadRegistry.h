#pragma once

#include <algorithm>
#include <cinttypes>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "nsLib/log.h"

// Connection metadata and stable ownership only. Input state and updating it
// belong to TGamepad, which each backend constructs lazily in GetGamepad().
template<class TGamepad>
class nsGamepadRegistry {
public:
    struct nsDevice {
        int nativeId;
        uint64_t connectionId;
        std::string name;
        int axisCount;
        int buttonCount;
        std::unique_ptr<TGamepad> gamepad;
    };

    bool Matches(int nativeId, const char* name, int axes, int buttons) const {
        const auto device = Find(nativeId);
        return device && device->axisCount == axes && device->buttonCount == buttons
            && device->name == GetLabel(name);
    }

    nsDevice& Connect(int nativeId, const char* name, int axes, int buttons) {
        if (Matches(nativeId, name, axes, buttons)) return *Find(nativeId);
        Disconnect(nativeId);
        auto device = std::make_unique<nsDevice>(nsDevice{
            nativeId, _nextConnectionId++, GetLabel(name),
            std::max(0, axes), std::max(0, buttons), nullptr});
        auto& result = *device;
        _devices.push_back(std::move(device));
        LogDevice("connected", result);
        return result;
    }

    void Disconnect(int nativeId) {
        for (auto it = _devices.begin(); it != _devices.end(); ++it) {
            if ((*it)->nativeId == nativeId) {
                LogDevice("disconnected", **it);
                _devices.erase(it);
                return;
            }
        }
    }

    void QueueConnect(int nativeId, const char* name, int axes, int buttons) {
        _changes.push_back({nativeId, true, GetLabel(name), axes, buttons});
    }

    void QueueDisconnect(int nativeId) {
        _changes.push_back({nativeId, false, {}, 0, 0});
    }

    void ApplyChanges() {
        for (const auto& change : _changes) {
            if (change.connected) Connect(change.id, change.name.c_str(), change.axes, change.buttons);
            else Disconnect(change.id);
        }
        _changes.clear();
    }

    void Clear() {
        _devices.clear();
        _changes.clear();
    }

    int GetCount() const { return static_cast<int>(_devices.size()); }

    nsDevice* GetDevice(int index) {
        return index >= 0 && index < GetCount() ? _devices[index].get() : nullptr;
    }

    const nsDevice* Find(int nativeId) const {
        for (const auto& device : _devices) {
            if (device->nativeId == nativeId) return device.get();
        }
        return nullptr;
    }

    nsDevice* Find(int nativeId) {
        return const_cast<nsDevice*>(std::as_const(*this).Find(nativeId));
    }

private:
    struct nsConnectionChange {
        int id;
        bool connected;
        std::string name;
        int axes;
        int buttons;
    };

    static const char* GetLabel(const char* name) {
        return name && *name ? name : "Unknown gamepad";
    }

    static void LogDevice(const char* event, const nsDevice& device) {
        Log::Info("Gamepad %s: id=%" PRIu64 " name=\"%s\" buttons=%d axes=%d",
            event, device.connectionId, device.name.c_str(), device.buttonCount, device.axisCount);
    }

    std::vector<std::unique_ptr<nsDevice>> _devices;
    std::vector<nsConnectionChange> _changes;
    uint64_t _nextConnectionId = 1;
};
