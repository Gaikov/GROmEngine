// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file UndoService.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "nsLib/SubSystem.h"
#include "UndoRedoOperation.h"

class nsUndoService : public nsSubSystem<nsUndoService> {
public:
    using StateId = std::uint64_t;

    bool Push(std::unique_ptr<nsUndoRedoOperation> operation, const std::string &name = "Edit");
    bool Push(nsUndoRedoOperation *operation, const std::string &name = "Edit");

    [[nodiscard]] bool HasUndo() const;
    [[nodiscard]] bool HasRedo() const;
    [[nodiscard]] const std::string &GetUndoName() const;
    [[nodiscard]] const std::string &GetRedoName() const;

    void Undo();
    void Redo();
    void Clear();

    void MarkClean();
    [[nodiscard]] bool IsDirty() const;
    [[nodiscard]] StateId GetCurrentStateId() const { return _currentStateId; }

protected:
    void OnRelease() override;

private:
    struct HistoryEntry {
        std::unique_ptr<nsUndoRedoOperation> operation;
        std::string name;
        StateId beforeStateId = 0;
        StateId afterStateId = 0;
    };

    using History = std::vector<HistoryEntry>;

    void ClearUndone();
    StateId NextStateId();

    History _done;
    History _undone;
    StateId _nextStateId = 0;
    StateId _currentStateId = 0;
    StateId _cleanStateId = 0;
};
