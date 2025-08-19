
// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file PacketsPool.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Core/memory/ObjectsPool.h"
#include "Networking/Packet.h"

class nsPacketsPool : public nsObjectsPool<nsPacketBuffer> {
public:
    nsPacketsPool();
    ~nsPacketsPool() override;

protected:
    nsPacketBuffer * CreateObject() override;
    void DestroyObject(nsPacketBuffer *object) override;
    void PrepareObject(nsPacketBuffer *object) override;
    void OnObjectRecycled(nsPacketBuffer *object) override;
};




