//
// Created by Roman on 3/27/2024.
//

#ifndef _GROM_BITMAPLOADER_H
#define _GROM_BITMAPLOADER_H

#include "Core/BitmapData.h"

class BitmapLoader {
public:
    static nsBitmapData::tSP LoadFromFile(const char *filePath);
};


#endif //_GROM_BITMAPLOADER_H
