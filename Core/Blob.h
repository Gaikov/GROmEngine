//
// Created by Roman on 4/3/2024.
//

#pragma once

#include "nsLib/comm_types.h"
#include "nsLib/StrTools.h"
#include "headers.h"

class nsFile {
public:
    explicit nsFile(uint size);

    virtual ~nsFile();

    uint GetSize() const { return _size; }

    uchar *GetData() { return _data; }

private:
    uint _size;
    uchar *_data;
};
