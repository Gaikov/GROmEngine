//
// Created by Roman on 1/21/2026.
//

#include "UndoInsertVisualChild.h"

nsUndoInsertVisualChild::nsUndoInsertVisualChild(nsVisualContainer2d *parent, nsVisualObject2d *child, const int index)
    : _parent(parent), _child(child), _index(index) {
}
void nsUndoInsertVisualChild::Init() {
    Redo();
}

void nsUndoInsertVisualChild::Redo() {
    _parent->AddChildAt(_child, _index);
}

void nsUndoInsertVisualChild::Undo() {
    _parent->RemoveChild(_child);
}
