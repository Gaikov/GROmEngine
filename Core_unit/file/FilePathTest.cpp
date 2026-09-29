#include "UnitCommon.h"

#include <filesystem>
#include <fstream>

#include "nsLib/FilePath.h"

namespace {
    namespace fs = std::filesystem;
}

TEST( FilePath, GetsModificationTime ) {
    const auto directory = fs::temp_directory_path() / "grom_file_path_time_test";
    const auto file = directory / "file.txt";
    std::error_code error;
    fs::remove_all( directory, error );
    ASSERT_FALSE( error );
    ASSERT_TRUE( fs::create_directories( directory, error ) );
    ASSERT_FALSE( error );

    const nsFilePath missing( ( directory / "missing.txt" ).string().c_str() );
    std::int64_t time = 123;
    EXPECT_FALSE( missing.GetModificationTime( time ) );
    EXPECT_EQ( 0, time );

    {
        std::ofstream stream( file );
        stream << "content";
    }
    const nsFilePath existing( file.string().c_str() );
    EXPECT_TRUE( existing.GetModificationTime( time ) );
    EXPECT_NE( 0, time );

    fs::remove_all( directory, error );
    EXPECT_FALSE( error );
}
