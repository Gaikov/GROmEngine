//
// Created by Roman Gaikov on 2019-10-08.
//
#include "UnitCommon.h"
#include "nsLib/FilePath.h"

TEST(FilePath, SingleFolder) {
    nsFilePath path("folder");

    ASSERT_TRUE(!path.IsExists());
    ASSERT_TRUE(path.CreateFolders());
    ASSERT_TRUE(path.IsExists());
    ASSERT_TRUE(path.IsFolder());
    ASSERT_TRUE(path.Remove());
}

TEST(FilePath, PathFolder) {
    nsFilePath path("folder/folder2/folder3");

    ASSERT_TRUE(!path.IsExists());
    ASSERT_TRUE(path.CreateFolders());
    ASSERT_TRUE(path.IsExists());
    ASSERT_TRUE(path.IsFolder());

    nsFilePath root("folder");
    ASSERT_TRUE(root.Remove());
    ASSERT_TRUE(!root.IsExists());
}

TEST(FilePath, NormalizePath) {
    nsString path("//\\\\/folder///folder2\\\\\\folder4//\\");
    nsFilePath::Normalize(path);

    ASSERT_TRUE(path == "/folder/folder2/folder4/");
}



