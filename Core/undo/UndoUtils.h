// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoUtils.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include <memory>

#include "UndoBatch.h"
#include "UndoPropertyChange.h"

class nsUndoUtils final {
public:
    template<typename TValue>
    static bool AddPropertyChange( nsUndoBatch &batch, nsProperty<TValue> &property, const TValue &nextValue ) {
        if ( property.GetValue() == nextValue ) return false;

        batch.Add( std::make_unique<nsUndoPropertyChange<TValue>>( property, nextValue ) );
        return true;
    }
};
