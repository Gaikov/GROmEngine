// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ChildRef.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/container/VisualContainer2d.h"
#include "Core/sys.h"

template<class TVisualClass>
class nsChildRef {
public:
    inline TVisualClass *operator->() const {
        return _visual;
    }

    nsChildRef(const char *childName) : _name(childName) {

    }

    inline operator TVisualClass* () const {
        return _visual;
    }

    void Init(nsVisualContainer2d *container, bool recursive = false) {
        nsVisualObject2d    *child;
        if (recursive) {
            child = container->GetChildByIdRecursive(_name.c_str());
        } else {
            child = container->GetChildById(_name.c_str());
        }

        if (!child) {
            Sys_FatalError("Child not found: %s", _name.c_str());
        }

        _visual = dynamic_cast<TVisualClass*>(child);
        if (!_visual) {
            Sys_FatalError("Invalid child class: %s", _name.c_str());
        }
    }

private:
    TVisualClass *_visual = nullptr;
    std::string _name;
};