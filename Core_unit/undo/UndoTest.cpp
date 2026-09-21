// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#include "UnitCommon.h"

#include <memory>
#include <vector>

#include "Core/debug/LogStdOut.h"
#include "Core/undo/UndoBatch.h"
#include "Core/undo/UndoService.h"
#include "Core/undo/UndoVarChange.h"
#include "Core/undo/UndoVectorAdd.h"
#include "Core/undo/UndoVectorInsert.h"
#include "Core/undo/UndoVectorRemove.h"
#include "Core/undo/UndoVectorRemoveAt.h"
#include "Core/undo/UndoVectorSet.h"
#include "Core/undo/array/UndoArrayAdd.h"
#include "Core/undo/array/UndoArrayRemove.h"
#include "nsLib/log.h"

namespace {
    LogStdOut out;

    class UndoFixture : public testing::Test {
    protected:
        void SetUp() override {
            Log::Init();
            Log::Shared()->AddPolicy(&out);
            nsUndoService::Init();
            nsUndoService::Shared()->Clear();
        }

        void TearDown() override {
            nsUndoService::Release();
            Log::Shared()->RemovePolicy(&out);
            Log::Release();
        }
    };

    class TrackedOperation final : public nsUndoRedoOperation {
    public:
        TrackedOperation(int &value, int newValue, int &destroyed)
            : _value(value), _newValue(newValue), _destroyed(destroyed) {
        }

        ~TrackedOperation() override { ++_destroyed; }
        void Init() override { _oldValue = _value; Redo(); }
        void Redo() override { _value = _newValue; }
        void Undo() override { _value = _oldValue; }

    private:
        int &_value;
        int _newValue;
        int &_destroyed;
        int _oldValue = 0;
    };
}

TEST_F(UndoFixture, SupportsRawAndUniqueOwnershipAndNames) {
    auto undo = nsUndoService::Shared();
    auto value = 0;
    auto destroyed = 0;
    EXPECT_TRUE(undo->Push(new TrackedOperation(value, 10, destroyed), "Raw edit"));
    EXPECT_TRUE(undo->Push(std::make_unique<TrackedOperation>(value, 20, destroyed), "Unique edit"));
    EXPECT_EQ(value, 20);
    EXPECT_EQ(undo->GetUndoName(), "Unique edit");
    undo->Clear();
    EXPECT_EQ(destroyed, 2);
}

TEST_F(UndoFixture, ClearsRedoBranchAfterDivergentPush) {
    auto undo = nsUndoService::Shared();
    auto value = 0;
    undo->Push(new nsUndoVarChange<int>(value, 1), "One");
    undo->Push(new nsUndoVarChange<int>(value, 2), "Two");
    undo->Undo();
    ASSERT_TRUE(undo->HasRedo());
    undo->Push(new nsUndoVarChange<int>(value, 3), "Three");
    EXPECT_EQ(value, 3);
    EXPECT_FALSE(undo->HasRedo());
    EXPECT_EQ(undo->GetUndoName(), "Three");
}

TEST_F(UndoFixture, CleanMarkerTracksUndoRedoAndDivergentState) {
    auto undo = nsUndoService::Shared();
    auto value = 0;
    undo->Push(new nsUndoVarChange<int>(value, 1), "First");
    undo->MarkClean();
    EXPECT_FALSE(undo->IsDirty());
    undo->Push(new nsUndoVarChange<int>(value, 2), "Second");
    EXPECT_TRUE(undo->IsDirty());
    undo->Undo();
    EXPECT_FALSE(undo->IsDirty());
    undo->Redo();
    EXPECT_TRUE(undo->IsDirty());
    undo->Undo();
    undo->Undo();
    undo->Push(new nsUndoVarChange<int>(value, 5), "Divergent");
    EXPECT_TRUE(undo->IsDirty());
}

TEST_F(UndoFixture, VectorOperationsPreserveItemsAtBounds) {
    auto undo = nsUndoService::Shared();
    std::vector<int> values = {1, 2};
    undo->Push(new nsUndoVectorInsert<int>(values, 2, 3));
    undo->Push(new nsUndoVectorAdd<int>(values, 4));
    undo->Push(new nsUndoVectorSet<int>(values, 0, 10));
    undo->Push(new nsUndoVectorRemove<int>(values, 2));
    undo->Push(new nsUndoVectorRemoveAt<int>(values, 1));
    EXPECT_EQ(values, (std::vector<int>{10, 4}));
    while (undo->HasUndo()) { undo->Undo(); }
    EXPECT_EQ(values, (std::vector<int>{1, 2}));
    while (undo->HasRedo()) { undo->Redo(); }
    EXPECT_EQ(values, (std::vector<int>{10, 4}));
}

TEST_F(UndoFixture, ArrayOperationsRestoreOriginalOrder) {
    auto undo = nsUndoService::Shared();
    nsArray<int> values;
    values.Add(1);
    values.Add(2);
    undo->Push(new nsUndoArrayAdd<int>(values, 3));
    undo->Push(new nsUndoArrayRemove<int>(values, 2));
    ASSERT_EQ(values.Size(), 2);
    EXPECT_EQ(values[0], 1);
    EXPECT_EQ(values[1], 3);
    undo->Undo();
    undo->Undo();
    ASSERT_EQ(values.Size(), 2);
    EXPECT_EQ(values[0], 1);
    EXPECT_EQ(values[1], 2);
}

TEST_F(UndoFixture, BatchReportsCountAndAppliesAsOneEntry) {
    auto value = 0;
    auto other = 0;
    auto batch = std::make_unique<nsUndoBatch>();
    EXPECT_TRUE(batch->IsEmpty());
    batch->Add(std::make_unique<nsUndoVarChange<int>>(value, 1));
    batch->Add(new nsUndoVarChange<int>(other, 2));
    EXPECT_EQ(batch->GetCount(), 2u);
    nsUndoService::Shared()->Push(std::move(batch), "Batch edit");
    EXPECT_EQ(value, 1);
    EXPECT_EQ(other, 2);
    nsUndoService::Shared()->Undo();
    EXPECT_EQ(value, 0);
    EXPECT_EQ(other, 0);
}
