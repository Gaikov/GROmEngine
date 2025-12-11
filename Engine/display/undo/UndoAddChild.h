// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoAddChild.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Core/undo/UndoRedoOperation.h"
#include "Engine/display/container/VisualContainer2d.h"

class nsUndoAddChild : public nsUndoRedoOperation {
public:
    nsUndoAddChild(nsVisualContainer2d *parent, nsVisualObject2d *child);
    void Init() override;
    void Redo() override;
    void Undo() override;

private:
    nsVisualContainer2d *_parent;
    nsVisualObject2d *_child;
};

