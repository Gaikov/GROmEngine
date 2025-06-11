// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file FixedUpdate.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

class nsFixedUpdateListener {
public:
    virtual ~nsFixedUpdateListener() = default;
    virtual void OnFixedUpdate(float deltaTime) = 0;
};

class nsFixedUpdate final {
public:
    explicit nsFixedUpdate(float fps, nsFixedUpdateListener *listener);
    void Update(float deltaTime);

    void SetFrameRate(float fps);
    float GetFrameRate() const;

private:
    nsFixedUpdateListener *_listener = nullptr;
    float _elapsedTime = 0.0f;
    float _fixedDeltaTime;
};
