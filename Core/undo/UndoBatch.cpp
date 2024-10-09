//
// Created by Roman on 6/24/2024.
//

#include "UndoBatch.h"

void nsUndoBatch::Init() {
    for (auto op : _list) {
        op->Init();
    }
}

void nsUndoBatch::Redo() {
    for (auto op : _list) {
        op->Redo();
    }
}

void nsUndoBatch::Undo() {
    for (auto it = _list.rbegin(); it != _list.rend(); it++) {
        (*it)->Undo();
    }
}

void nsUndoBatch::Add(nsUndoRedoOperation *op) {
    assert(std::find(_list.begin(), _list.end(), op) == _list.end());
    _list.push_back(op);
}

nsUndoBatch::~nsUndoBatch() {
    for (auto op : _list) {
        delete op;
    }
}
