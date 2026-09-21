//
// Created by Roman on 6/24/2024.
//

#include "UndoBatch.h"

#include <algorithm>
#include <cassert>

void nsUndoBatch::Init() {
    assert(!IsEmpty());
    for (const auto &operation : _operations) {
        operation->Init();
    }
}

void nsUndoBatch::Redo() {
    for (const auto &operation : _operations) {
        operation->Redo();
    }
}

void nsUndoBatch::Undo() {
    for (auto it = _operations.rbegin(); it != _operations.rend(); ++it) {
        (*it)->Undo();
    }
}

void nsUndoBatch::Add(std::unique_ptr<nsUndoRedoOperation> operation) {
    assert(operation);
    if (operation) {
        _operations.push_back(std::move(operation));
    }
}

void nsUndoBatch::Add(nsUndoRedoOperation *operation) {
    Add(std::unique_ptr<nsUndoRedoOperation>(operation));
}
