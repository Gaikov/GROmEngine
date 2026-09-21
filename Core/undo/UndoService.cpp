// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoService.cpp
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------

#include "UndoService.h"

#include <cassert>
#include <utility>

#include "nsLib/log.h"

namespace {
    const std::string EMPTY_OPERATION_NAME;
}

//---------------------------------------------------------
// nsUndoService::Push:
//---------------------------------------------------------
bool nsUndoService::Push(std::unique_ptr<nsUndoRedoOperation> operation, const std::string &name) {
    assert(operation);
    if (!operation) {
        return false;
    }

    const auto beforeStateId = _currentStateId;
    operation->Init();
    ClearUndone();

    HistoryEntry entry;
    entry.operation = std::move(operation);
    entry.name = name.empty() ? "Edit" : name;
    entry.beforeStateId = beforeStateId;
    entry.afterStateId = NextStateId();
    _currentStateId = entry.afterStateId;
    _done.push_back(std::move(entry));
    return true;
}

//---------------------------------------------------------
// nsUndoService::Push: compatibility ownership overload
//---------------------------------------------------------
bool nsUndoService::Push(nsUndoRedoOperation *operation, const std::string &name) {
    return Push(std::unique_ptr<nsUndoRedoOperation>(operation), name);
}

//---------------------------------------------------------
// nsUndoService::HasUndo:
//---------------------------------------------------------
bool nsUndoService::HasUndo() const {
    return !_done.empty();
}

//---------------------------------------------------------
// nsUndoService::HasRedo:
//---------------------------------------------------------
bool nsUndoService::HasRedo() const {
    return !_undone.empty();
}

//---------------------------------------------------------
// nsUndoService::GetUndoName:
//---------------------------------------------------------
const std::string &nsUndoService::GetUndoName() const {
    return HasUndo() ? _done.back().name : EMPTY_OPERATION_NAME;
}

//---------------------------------------------------------
// nsUndoService::GetRedoName:
//---------------------------------------------------------
const std::string &nsUndoService::GetRedoName() const {
    return HasRedo() ? _undone.back().name : EMPTY_OPERATION_NAME;
}

//---------------------------------------------------------
// nsUndoService::Undo:
//---------------------------------------------------------
void nsUndoService::Undo() {
    if (!HasUndo()) {
        return;
    }

    auto entry = std::move(_done.back());
    _done.pop_back();
    entry.operation->Undo();
    _currentStateId = entry.beforeStateId;
    _undone.push_back(std::move(entry));
}

//---------------------------------------------------------
// nsUndoService::Redo:
//---------------------------------------------------------
void nsUndoService::Redo() {
    if (!HasRedo()) {
        return;
    }

    auto entry = std::move(_undone.back());
    _undone.pop_back();
    entry.operation->Redo();
    _currentStateId = entry.afterStateId;
    _done.push_back(std::move(entry));
}

//---------------------------------------------------------
// nsUndoService::Clear:
//---------------------------------------------------------
void nsUndoService::Clear() {
    _done.clear();
    ClearUndone();
    _currentStateId = NextStateId();
    _cleanStateId = _currentStateId;
}

//---------------------------------------------------------
// nsUndoService::MarkClean:
//---------------------------------------------------------
void nsUndoService::MarkClean() {
    _cleanStateId = _currentStateId;
}

//---------------------------------------------------------
// nsUndoService::IsDirty:
//---------------------------------------------------------
bool nsUndoService::IsDirty() const {
    return _currentStateId != _cleanStateId;
}

//---------------------------------------------------------
// nsUndoService::OnRelease:
//---------------------------------------------------------
void nsUndoService::OnRelease() {
    Log::Info("...releasing undo service");
    Clear();
    nsSubSystem::OnRelease();
}

//---------------------------------------------------------
// nsUndoService::ClearUndone:
//---------------------------------------------------------
void nsUndoService::ClearUndone() {
    _undone.clear();
}

//---------------------------------------------------------
// nsUndoService::NextStateId:
//---------------------------------------------------------
nsUndoService::StateId nsUndoService::NextStateId() {
    return ++_nextStateId;
}
