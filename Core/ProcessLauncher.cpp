// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#include "ProcessLauncher.h"

#include <filesystem>

#if defined( _WIN32 )
#include <windows.h>
#elif !defined( WEB_ASM ) && !defined( ANDROID )
#include <spawn.h>
#include <sys/wait.h>
#include <thread>
#endif

namespace {
#if defined( _WIN32 )
    std::wstring QuoteWindowsArgument( const std::wstring &argument ) {
        if ( argument.find_first_of( L" \t\"" ) == std::wstring::npos ) return argument;
        std::wstring result = L"\"";
        auto slashes = size_t( 0 );
        for ( const auto character : argument ) {
            if ( character == L'\\' ) {
                ++slashes;
            } else if ( character == L'\"' ) {
                result.append( slashes * 2 + 1, L'\\' );
                result += character;
                slashes = 0;
            } else {
                result.append( slashes, L'\\' );
                slashes = 0;
                result += character;
            }
        }
        result.append( slashes * 2, L'\\' );
        result += L'\"';
        return result;
    }
#endif
}

bool nsProcessLauncher::LaunchDetached( const nsProcessLaunchInfo &info ) {
#if defined( WEB_ASM ) || defined( ANDROID )
    return false;
#else
    if ( info.executable.empty() || info.workingDirectory.empty() ) return false;
    std::error_code error;
    if ( !std::filesystem::is_regular_file( info.executable, error ) || error ) return false;
    if ( !std::filesystem::is_directory( info.workingDirectory, error ) || error ) return false;

#if defined( _WIN32 )
    const auto executable = std::filesystem::u8path( info.executable ).wstring();
    auto commandLine = QuoteWindowsArgument( executable );
    for ( const auto &argument : info.arguments ) {
        commandLine += L' ';
        commandLine += QuoteWindowsArgument( std::filesystem::u8path( argument ).wstring() );
    }
    auto mutableCommandLine = std::vector<wchar_t>( commandLine.begin(), commandLine.end() );
    mutableCommandLine.push_back( L'\0' );
    STARTUPINFOW startup = {};
    startup.cb = sizeof( startup );
    PROCESS_INFORMATION process = {};
    const auto workingDirectory = std::filesystem::u8path( info.workingDirectory ).wstring();
    const auto launched = CreateProcessW(
        executable.c_str(), mutableCommandLine.data(), nullptr, nullptr, FALSE, 0, nullptr,
        workingDirectory.c_str(), &startup, &process );
    if ( !launched ) return false;
    CloseHandle( process.hThread );
    CloseHandle( process.hProcess );
    return true;
#else
    auto arguments = std::vector<char *>();
    arguments.reserve( info.arguments.size() + 2 );
    arguments.push_back( const_cast<char *>( info.executable.c_str() ) );
    for ( const auto &argument : info.arguments ) {
        arguments.push_back( const_cast<char *>( argument.c_str() ) );
    }
    arguments.push_back( nullptr );

    posix_spawn_file_actions_t actions;
    if ( posix_spawn_file_actions_init( &actions ) != 0 ) return false;
    const auto chdirResult = posix_spawn_file_actions_addchdir_np(
        &actions, info.workingDirectory.c_str() );
    if ( chdirResult != 0 ) {
        posix_spawn_file_actions_destroy( &actions );
        return false;
    }
    pid_t process = 0;
    extern char **environ;
    const auto result = posix_spawn(
        &process, info.executable.c_str(), &actions, nullptr, arguments.data(), environ );
    posix_spawn_file_actions_destroy( &actions );
    if ( result != 0 ) return false;
    std::thread( [process] { waitpid( process, nullptr, 0 ); } ).detach();
    return true;
#endif
#endif
}
