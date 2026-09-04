cmake_minimum_required(VERSION 3.10)
project(Http)

# Native platforms without a complete OS HTTP API use the vendored curl. Apple,
# Android and Web use NSURLSession, HttpURLConnection and fetch respectively,
# avoiding an extra TLS dependency on those platforms.
if (NOT APPLE AND NOT WEB_ASM AND NOT CMAKE_SYSTEM_NAME STREQUAL Android)
    # curl uses a few generic cache option names. Preserve values supplied by
    # the parent game so configuring the HTTP module does not affect later
    # targets in the engine.
    foreach (_generic_option BUILD_SHARED_LIBS BUILD_STATIC_LIBS BUILD_TESTING)
        get_property(_${_generic_option}_type CACHE ${_generic_option} PROPERTY TYPE)
        if (_${_generic_option}_type)
            set(_${_generic_option}_was_set TRUE)
            set(_${_generic_option}_value "${${_generic_option}}")
        else ()
            set(_${_generic_option}_was_set FALSE)
        endif ()
    endforeach ()

    set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_LIBCURL_DOCS OFF CACHE BOOL "" FORCE)
    set(BUILD_MISC_DOCS OFF CACHE BOOL "" FORCE)
    set(CURL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
    set(CURL_ENABLE_EXPORT_TARGET OFF CACHE BOOL "" FORCE)
    set(CURL_USE_PKGCONFIG OFF CACHE BOOL "" FORCE)

    # Keep authentication, proxies, cookies, MIME and WebSockets available for
    # future HTTP users while excluding unrelated application protocols.
    foreach (_curl_protocol DICT FILE FTP GOPHER IMAP LDAP LDAPS MQTT POP3 RTSP SMTP TELNET TFTP)
        set(CURL_DISABLE_${_curl_protocol} ON CACHE BOOL "" FORCE)
    endforeach ()
    set(CURL_DISABLE_IPFS ON CACHE BOOL "" FORCE)
    set(CURL_ENABLE_SMB OFF CACHE BOOL "" FORCE)
    set(CURL_USE_LIBSSH2 OFF CACHE BOOL "" FORCE)
    set(CURL_USE_LIBSSH OFF CACHE BOOL "" FORCE)
    set(CURL_USE_GSSAPI OFF CACHE BOOL "" FORCE)
    set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
    set(USE_LIBIDN2 OFF CACHE BOOL "" FORCE)
    set(USE_NGHTTP2 OFF CACHE BOOL "" FORCE)
    set(CURL_ZLIB OFF CACHE STRING "" FORCE)
    set(CURL_BROTLI OFF CACHE STRING "" FORCE)
    set(CURL_ZSTD OFF CACHE STRING "" FORCE)

    if (WIN32)
        set(CURL_USE_SCHANNEL ON CACHE BOOL "" FORCE)
        set(CURL_USE_OPENSSL OFF CACHE BOOL "" FORCE)
    else ()
        set(CURL_USE_SCHANNEL OFF CACHE BOOL "" FORCE)
        set(CURL_USE_OPENSSL ON CACHE BOOL "" FORCE)
    endif ()

    add_subdirectory(../ThirdParty/curl ${CMAKE_CURRENT_BINARY_DIR}/curl EXCLUDE_FROM_ALL)

    foreach (_generic_option BUILD_SHARED_LIBS BUILD_STATIC_LIBS BUILD_TESTING)
        if (_${_generic_option}_was_set)
            set(${_generic_option} "${_${_generic_option}_value}"
                    CACHE ${_${_generic_option}_type} "" FORCE)
        else ()
            unset(${_generic_option} CACHE)
        endif ()
    endforeach ()
endif ()

set(sources
        HttpClient.cpp
        HttpClient.h
        HttpBackend.h
)

if (CMAKE_SYSTEM_NAME STREQUAL Android)
    list(APPEND sources backends/AndroidHttpBackend.cpp)
elseif (WEB_ASM)
    list(APPEND sources backends/WebHttpBackend.cpp)
elseif (APPLE)
    list(APPEND sources backends/AppleHttpBackend.mm)
else ()
    list(APPEND sources backends/CurlHttpBackend.cpp)
endif ()

add_library(Http STATIC ${sources})
target_include_directories(Http PUBLIC ..)
target_link_libraries(Http PUBLIC nsLib)

if (APPLE)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    target_link_libraries(Http PRIVATE ${FOUNDATION_FRAMEWORK})
elseif (NOT WEB_ASM AND NOT CMAKE_SYSTEM_NAME STREQUAL Android)
    target_link_libraries(Http PRIVATE CURL::libcurl)
endif ()
