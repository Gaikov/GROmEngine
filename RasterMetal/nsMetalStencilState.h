//
// Created by OpenCode
//
#pragma once

#include "Engine/RenDevice.h"
#include "nsLib/StrTools.h"

#include <array>

#import <Metal/Metal.h>

class nsMetalStencilState : public IStencilState {
public:
    static nsMetalStencilState* Load(id<MTLDevice> device, const char *fileName);

    explicit nsMetalStencilState(id<MTLDevice> device = nil) : _device(device) {}
    const char *GetPath() const { return _fileName; }
    void SetStencilRef(byte value) override;
    void Apply(id<MTLRenderCommandEncoder> encoder, bool zEnable, bool zWrite);
    void Invalidate();
    MTLColorWriteMask GetColorWriteMask() const { return _colorWriteMask; }

private:
    id<MTLDevice> _device = nil;
    nsString _fileName;
    bool _enabled = false;
    MTLStencilOperation _opFail = MTLStencilOperationKeep;
    MTLStencilOperation _opZFail = MTLStencilOperationKeep;
    MTLStencilOperation _opZPass = MTLStencilOperationKeep;
    MTLCompareFunction _func = MTLCompareFunctionAlways;
    uint32_t _funcRef = 0;
    uint32_t _funcMask = 0xff;
    uint32_t _stencilMask = 0xff;
    MTLColorWriteMask _colorWriteMask = MTLColorWriteMaskAll;
    std::array<id<MTLDepthStencilState>, 4> _states{};

    bool Reload();
    id<MTLDepthStencilState> GetOrCreateState(bool zEnable, bool zWrite);
};
