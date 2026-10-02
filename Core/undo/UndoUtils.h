// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoUtils.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include <memory>

#include "UndoBatch.h"
#include "UndoPropertyChange.h"
#include "nsLib/Vec3.h"
#include "nsLib/color.h"

class nsUndoUtils final {
public:
    template<typename TValue>
    static bool AddPropertyChange( nsUndoBatch &batch, nsProperty<TValue> &property, const TValue &nextValue ) {
        if ( ValuesEqual( property.GetValue(), nextValue ) ) return false;

        batch.Add( std::make_unique<nsUndoPropertyChange<TValue>>( property, nextValue ) );
        return true;
    }

private:
    template<typename TValue>
    static bool ValuesEqual( const TValue &left, const TValue &right ) {
        return left == right;
    }

    static bool ValuesEqual( const nsVec3 &left, const nsVec3 &right ) {
        return left.x == right.x && left.y == right.y && left.z == right.z;
    }

    static bool ValuesEqual( const nsColor &left, const nsColor &right ) {
        return left.r == right.r && left.g == right.g
            && left.b == right.b && left.a == right.a;
    }
};
