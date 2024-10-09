//
// Created by Roman on 6/24/2024.
//
#include "UnitCommon.h"
#include "Core/undo/UndoService.h"
#include "Core/undo/UndoVarChange.h"
#include "nsLib/log.h"
#include "Core/debug/LogStdOut.h"
#include "Core/undo/UndoVectorInsert.h"

static LogStdOut   out;

static void UndoInit() {
    Log::Init();
    Log::Shared()->AddPolicy(&out);
    nsUndoService::Init();
}

static void UndoRelease() {
    nsUndoService::Release();
    Log::Release();
}

TEST(UndoService, Var) {
    UndoInit();
    int var = 0;

    auto service = nsUndoService::Shared();

    service->Push(new nsUndoVarChange<int>(var, 10));
    ASSERT_TRUE(var == 10);

    service->Undo();
    ASSERT_TRUE(var == 0);

    service->Redo();
    ASSERT_TRUE(var == 10);

    service->Push(new nsUndoVarChange<int>(var, 20));
    ASSERT_TRUE(var == 20);

    service->Undo();
    ASSERT_TRUE(var == 10);

    service->Undo();
    ASSERT_TRUE(var == 0);

    UndoRelease();
}

TEST(UndoService, UndoVectorInsert) {
    UndoInit();
    auto undo = nsUndoService::Shared();

    auto    *list = new std::vector<int>();
    std::vector<int> &v = *list;
    v.push_back(1);
    v.push_back(2);

    undo->Push(new nsUndoVectorInsert<int>(v, 2, 10));
    ASSERT_TRUE(v[0] == 1);
    ASSERT_TRUE(v[1] == 2);

    ASSERT_TRUE(v.size() == 3);
    ASSERT_TRUE(v[2] == 10);

    undo->Undo();
    ASSERT_TRUE(v.size() == 2);
    ASSERT_TRUE(v[0] == 1);
    ASSERT_TRUE(v[1] == 2);

    undo->Redo();
    undo->Push(new nsUndoVectorInsert<int>(v, 0, 10));
    ASSERT_TRUE(v.size() == 4);
    ASSERT_TRUE(v[0] == 10);
    ASSERT_TRUE(v[3] == 10);

    undo->Undo();
    undo->Undo();
    ASSERT_TRUE(v.size() == 2);
    ASSERT_TRUE(v[0] == 1);
    ASSERT_TRUE(v[1] == 2);

    UndoRelease();
}