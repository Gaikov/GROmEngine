//
// Created by Roman on 6/11/2025.
//

#include "FixedUpdate.h"

nsFixedUpdate::nsFixedUpdate(float fps,
                             nsFixedUpdateListener *listener)
    : _listener(listener),
      _fixedDeltaTime(1.0f / fps) {
}

void nsFixedUpdate::Update(float deltaTime) {
    _elapsedTime += deltaTime;
    if (_elapsedTime > _fixedDeltaTime) {
        const float amount = _elapsedTime / _fixedDeltaTime;
        const int count = static_cast<int>(amount);
        _elapsedTime -= static_cast<float>(count) * _fixedDeltaTime;
        for (int i = 0; i < count; ++i) {
            _listener->OnFixedUpdate(_fixedDeltaTime);
        }
    }
}

void nsFixedUpdate::SetFrameRate(const float fps) {
    _fixedDeltaTime = 1.0f / fps;
}

float nsFixedUpdate::GetFrameRate() const {
    return 1.0f / _fixedDeltaTime;
}

