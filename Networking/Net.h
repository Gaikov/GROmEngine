// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Net.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socklen_t;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <thread>
#include <vector>

struct nsNet final {
    static bool Init();
    static void Release();
    static int GetLastError();
    static const char* GetErrorMessage(int code);
    static bool CheckLastError(const char* message);
    static void CloseSocket(int socket);
};

#define NET_ERROR(message, result) if (!nsNet::CheckLastError(message)) return result;
