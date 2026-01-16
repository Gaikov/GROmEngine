// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file StringWriter.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Core/DataWriter.h"

class nsStringWriter : public IDataWriter {
public:
    bool Printf(const char *fmt, ...) override;
    bool Write(const void *data, uint size) override;
    bool IsValid() override;
    bool Seek(long offset, int origin) override;
    long Tell() override;

    const std::string &GetBuffer() const { return _buffer; }

private:
    std::string _buffer;
    long _pos = 0;
};
