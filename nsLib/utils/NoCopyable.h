// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file NoCopyable.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

class nsNoCopyable {
public:
    nsNoCopyable() = default;
    virtual ~nsNoCopyable() = default;

    nsNoCopyable(const nsNoCopyable &) = delete;
    nsNoCopyable(nsNoCopyable &) = delete;
    nsNoCopyable &operator=(const nsNoCopyable &) = delete;
    nsNoCopyable &operator=(nsNoCopyable &) = delete;

    nsNoCopyable(nsNoCopyable &&) = delete;
    nsNoCopyable &operator=(nsNoCopyable &&) = delete;
};
