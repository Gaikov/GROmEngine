// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoInsertVisualChild.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Core/undo/UndoRedoOperation.h"
#include "Engine/display/container/VisualContainer2d.h"

class nsUndoInsertVisualChild final : public nsUndoRedoOperation {
public:
    nsUndoInsertVisualChild(nsVisualContainer2d *parent, nsVisualObject2d *child, int index);
    void Init() override;
    void Redo() override;
    void Undo() override;

private:
    nsVisualContainer2d *_parent;
    nsVisualObject2d *_child;
    int _index;
};
