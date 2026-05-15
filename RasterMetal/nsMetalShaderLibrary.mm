//
// Created by OpenCode
//

#include "nsMetalShaderLibrary.h"
#include "Core/Package.h"
#include "nsLib/log.h"

bool nsMetalShaderLibrary::CompileLibraryFromSource(const char *source) {
    Invalidate();

    NSError *error = nil;
    NSString *src = [NSString stringWithUTF8String:source];
    _library = [_device newLibraryWithSource:src options:nil error:&error];
    if (!_library) {
        Log::Error("Metal: failed to compile shader library: %s",
                   [[error localizedDescription] UTF8String]);
        return false;
    }
    return true;
}

bool nsMetalShaderLibrary::CompileLibraryFromFile(const char *filePath) {
    Log::Info("...loading Metal shader: %s", filePath);
    const auto file = g_pack.LoadFile(filePath);
    if (!file) {
        Log::Error("Metal: failed to load shader file: %s", filePath);
        return false;
    }

    const auto src = reinterpret_cast<const char *>(file->GetData());
    const bool result = CompileLibraryFromSource(src);
    g_pack.ReleaseFile(file);
    return result;
}

void nsMetalShaderLibrary::Invalidate() {
    _library = nil;
}

id<MTLFunction> nsMetalShaderLibrary::GetFunction(const char *name) const {
    if (!_library) return nil;
    NSString *funcName = [NSString stringWithUTF8String:name];
    return [_library newFunctionWithName:funcName];
}
