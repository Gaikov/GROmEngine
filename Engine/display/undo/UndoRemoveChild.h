// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoRemoveChild.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Core/undo/UndoRedoOperation.h"
#include "Engine/display/VisualObject2d.h"

class nsUndoRemoveChild final : public nsUndoRedoOperation {
public:
    explicit nsUndoRemoveChild(nsVisualObject2d *obj);
    void Init() override;
    void Redo() override;
    void Undo() override;

private:
    nsVisualObject2d *_obj;
    nsVisualContainer2d *_parent;
    int _index;
};
