//
// Created by OpenCode
//

#include "nsMetalStencilState.h"
#include "Core/ParseFile.h"
#include "Core/ParserUtils.h"
#include "nsLib/log.h"

static MTLStencilOperation MapStencilOp(const char *name) {
    if (StrEqual(name, "zero")) return MTLStencilOperationZero;
    if (StrEqual(name, "replace")) return MTLStencilOperationReplace;
    if (StrEqual(name, "incr")) return MTLStencilOperationIncrementClamp;
    if (StrEqual(name, "decr")) return MTLStencilOperationDecrementClamp;
    if (StrEqual(name, "invert")) return MTLStencilOperationInvert;
    return MTLStencilOperationKeep;
}

static MTLCompareFunction MapCompare(const char *name) {
    if (StrEqual(name, "never")) return MTLCompareFunctionNever;
    if (StrEqual(name, "less")) return MTLCompareFunctionLess;
    if (StrEqual(name, "equal")) return MTLCompareFunctionEqual;
    if (StrEqual(name, "lequal")) return MTLCompareFunctionLessEqual;
    if (StrEqual(name, "greater")) return MTLCompareFunctionGreater;
    if (StrEqual(name, "notequal")) return MTLCompareFunctionNotEqual;
    if (StrEqual(name, "gequal")) return MTLCompareFunctionGreaterEqual;
    return MTLCompareFunctionAlways;
}

nsMetalStencilState *nsMetalStencilState::Load(id<MTLDevice> device, const char *fileName) {
    if (!StrCheck(fileName)) return nullptr;
    auto state = new nsMetalStencilState(device);
    state->_fileName = fileName;
    if (!state->Reload()) {
        delete state;
        return nullptr;
    }
    return state;
}

bool nsMetalStencilState::Reload() {
    Invalidate();
    nsParseFile pf;
    auto ss = pf.BeginFile(_fileName);
    if (!ss) {
        Log::Warning("can't load stencil state: %s", _fileName.AsChar());
        return false;
    }

    _enabled = ParseFloat(ss, "enabled") > 0;
    _opFail = MapStencilOp(ParseStrP(ss, "op_fail", "keep"));
    _opZFail = MapStencilOp(ParseStrP(ss, "op_zfail", "keep"));
    _opZPass = MapStencilOp(ParseStrP(ss, "op_zpass", "keep"));
    _func = MapCompare(ParseStrP(ss, "func", "always"));
    _funcRef = (uint32_t)ParseFloat(ss, "func_ref", 0);
    _funcMask = (uint32_t)ParseFloat(ss, "func_mask", 0xff);
    _stencilMask = (uint32_t)ParseFloat(ss, "stencil_mask", 0xff);

    float v[4] = {1, 1, 1, 1};
    if (ParseFloat4(ss, "color_mask", v)) {
        _colorWriteMask = 0;
        if (v[0] > 0) _colorWriteMask |= MTLColorWriteMaskRed;
        if (v[1] > 0) _colorWriteMask |= MTLColorWriteMaskGreen;
        if (v[2] > 0) _colorWriteMask |= MTLColorWriteMaskBlue;
        if (v[3] > 0) _colorWriteMask |= MTLColorWriteMaskAlpha;
    }

    return true;
}

void nsMetalStencilState::SetStencilRef(byte value) {
    _funcRef = value;
}

id<MTLDepthStencilState> nsMetalStencilState::GetOrCreateState(bool zEnable, bool zWrite) {
    const size_t index = (zEnable ? 2 : 0) | (zEnable && zWrite ? 1 : 0);
    if (_states[index]) return _states[index];

    MTLDepthStencilDescriptor *desc = [[MTLDepthStencilDescriptor alloc] init];
    desc.depthCompareFunction = zEnable ? MTLCompareFunctionLessEqual : MTLCompareFunctionAlways;
    desc.depthWriteEnabled = zEnable && zWrite;

    if (_enabled) {
        MTLStencilDescriptor *stencil = [[MTLStencilDescriptor alloc] init];
        stencil.stencilCompareFunction = _func;
        stencil.stencilFailureOperation = _opFail;
        stencil.depthFailureOperation = _opZFail;
        stencil.depthStencilPassOperation = _opZPass;
        stencil.readMask = _funcMask;
        stencil.writeMask = _stencilMask;
        desc.frontFaceStencil = stencil;
        desc.backFaceStencil = stencil;
    }

    _states[index] = [_device newDepthStencilStateWithDescriptor:desc];
    return _states[index];
}

void nsMetalStencilState::Invalidate() {
    _states.fill(nil);
}

void nsMetalStencilState::Apply(id<MTLRenderCommandEncoder> encoder, bool zEnable, bool zWrite) {
    auto state = GetOrCreateState(zEnable, zWrite);
    if (state) {
        [encoder setDepthStencilState:state];
    }
    [encoder setStencilReferenceValue:_funcRef];
}
