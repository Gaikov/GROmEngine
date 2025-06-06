// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ClientSocket.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Networking/common/BaseClientSocket.h"


class nsClientSocket : public nsBaseClientSocket {
public:
    bool Open();
    bool Connect(const char *ip, int port) const;
};
