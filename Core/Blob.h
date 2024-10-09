//
// Created by Roman on 4/3/2024.
//

#ifndef _GROM_BLOB_H
#define _GROM_BLOB_H

#include "nsLib/comm_types.h"
#include "nsLib/StrTools.h"
#include "headers.h"

class nsFile {
public:
    explicit nsFile(uint size);

    virtual ~nsFile();

    uint GetSize() const { return m_size; }

    uchar *GetData() { return m_data; }

private:
    uint m_size;
    uchar *m_data;
};

#endif //_GROM_BLOB_H
