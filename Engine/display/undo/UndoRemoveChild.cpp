//
// Created by Roman on 12/11/2025.
//

#include "UndoRemoveChild.h"
#include "display/container/VisualContainer2d.h"

nsUndoRemoveChild::nsUndoRemoveChild(nsVisualObject2d *obj) : _obj(obj), _index(-1) {
    _parent = obj->GetParent();
    assert(_parent);
}

void nsUndoRemoveChild::Init() {
    _index = _parent->GetChildIndex(_obj);
    Redo();
}

void nsUndoRemoveChild::Redo() {
    _parent->RemoveChild(_obj);
}

void nsUndoRemoveChild::Undo() {
    _parent->AddChildAt(_obj, _index);
}
