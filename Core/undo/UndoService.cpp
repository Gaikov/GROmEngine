//
// Created by Roman on 6/24/2024.
//

#include "UndoService.h"
#include "nsLib/log.h"

void nsUndoService::Push(nsUndoRedoOperation *op) {
    _doneList.push_back(op);
    op->Init();
    ClearUnDone();
}
bool nsUndoService::HasUndo() const {
    return !_doneList.empty();
}

void nsUndoService::Undo() {
    if (HasUndo()) {
        auto last = _doneList[_doneList.size() - 1];
        _doneList.pop_back();
        _unDoneList.push_back(last);
        last->Undo();
    }
}

bool nsUndoService::HasRedo() const {
    return !_unDoneList.empty();
}

void nsUndoService::Redo() {
    if (HasRedo()) {
        auto last = _unDoneList[_unDoneList.size() - 1];
        _unDoneList.pop_back();
        _doneList.push_back(last);
        last->Redo();
    }
}

void nsUndoService::OnRelease() {
    Log::Info("...releasing undo service");
    Clear();
    nsSubSystem::OnRelease();
}

void nsUndoService::Clear() {
    for (auto op : _doneList) {
        delete op;
    }
    _doneList.clear();
    ClearUnDone();
}

void nsUndoService::ClearUnDone() {
    for (auto op : _unDoneList) {
        delete op;
    }
    _unDoneList.clear();
}
