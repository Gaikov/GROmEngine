// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualCustomProp.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Core/serialization/Serializable.h"

class nsVisualCustomProp final {
    friend class nsVisualCustomPropFactory;
public:
    using sp_t = std::shared_ptr<nsVisualCustomProp>;

    enum Type {
        NONE,
        STRING,
        ENUM,
        BOOL,
        NUMBER
    };


    [[nodiscard]] Type GetType() const { return _type; }
    [[nodiscard]] nsSerializable *GetValue() const { return _value.get(); }

private:
    nsVisualCustomProp(Type type, nsSerializable *value);

    Type _type = NONE;
    std::unique_ptr<nsSerializable> _value;
};



