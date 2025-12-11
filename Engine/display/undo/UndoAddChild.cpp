//
// Created by Roman on 12/11/2025.
//

#include "UndoAddChild.h"

nsUndoAddChild::nsUndoAddChild(nsVisualContainer2d *parent, nsVisualObject2d *child) : _parent(parent), _child(child) {
}

void nsUndoAddChild::Init() {
    Redo();
}

void nsUndoAddChild::Redo() {
    _parent->AddChild(_child);
}

void nsUndoAddChild::Undo() {
    _parent->RemoveChild(_child);
}
