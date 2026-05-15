//
// Created by OpenCode
//
#pragma once

#include <Metal/Metal.h>
#include <string>

class nsMetalShaderLibrary {
public:
    nsMetalShaderLibrary(id<MTLDevice> device) : _device(device) {}

    bool CompileLibraryFromSource(const char *source);
    bool CompileLibraryFromFile(const char *filePath);
    void Invalidate();

    id<MTLLibrary> GetLibrary() const { return _library; }
    id<MTLFunction> GetFunction(const char *name) const;

private:
    id<MTLDevice>   _device   = nil;
    id<MTLLibrary>  _library  = nil;
};
