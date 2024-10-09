//
// Created by Roman on 5/24/2024.
//

#ifndef _GROM_ANDROIDSOFTINPUT_H
#define _GROM_ANDROIDSOFTINPUT_H

#include "Core/SmartPtr.h"

class ISoftInput {
public:
    typedef nsSmartPtr<ISoftInput> sp_t;

    virtual ~ISoftInput() = default;

    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual bool IsActive() = 0;
};

#endif //_GROM_ANDROIDSOFTINPUT_H
