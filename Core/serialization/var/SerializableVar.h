// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SerializableVar.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Core/serialization/Serializable.h"
#include "nsLib/models/Property.h"

template<typename TVar>
class nsSerializableVar : public nsProperty<TVar>, public nsSerializable {
public:
    explicit nsSerializableVar(const char *name, TVar defValue)
        : nsProperty<TVar>(defValue), nsSerializable(name) {
    }

    void ResetDefault() override {
        nsProperty<TVar>::Reset();
    }
};
