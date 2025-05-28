// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Net.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socklen_t;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

struct nsNet final {
    static bool Init();
    static void Release();
    static int GetLastError();
    static const char* GetErrorMessage(int code);
    static bool CheckLastError();
};
