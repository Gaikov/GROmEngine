//
// Created by OpenCode
//

#include "nsMetalProgram.h"
#include "nsMetalShaderLibrary.h"
#include "Engine/RenDevice.h"
#include "nsLib/log.h"
#include <cstring>

nsMetalProgram::nsMetalProgram(id<MTLDevice> device) : _device(device) {
    _shaderLibrary = new nsMetalShaderLibrary(device);
    memset(&_uniforms, 0, sizeof(_uniforms));
}

nsMetalProgram::~nsMetalProgram() {
    Unload();
    delete _shaderLibrary;
}

bool nsMetalProgram::Load(const char *vertexShaderPath, const char *fragmentShaderPath) {
    Unload();

    Log::Info("...loading Metal program: %s + %s", vertexShaderPath, fragmentShaderPath);

    if (!_shaderLibrary->CompileLibraryFromFile(vertexShaderPath)) {
        return false;
    }

    auto vertexFunc = _shaderLibrary->GetFunction("vertexMain");
    auto fragmentFunc = _shaderLibrary->GetFunction("pixelMain");
    if (!vertexFunc || !fragmentFunc) {
        Log::Error("Metal: failed to get shader functions");
        return false;
    }
    _vertexFunction = vertexFunc;
    _fragmentFunction = fragmentFunc;

    auto descriptor = [[MTLRenderPipelineDescriptor alloc] init];
    descriptor.vertexFunction = vertexFunc;
    descriptor.fragmentFunction = fragmentFunc;
    descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    descriptor.colorAttachments[0].blendingEnabled = YES;
    descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
    descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;

    auto vertexDesc = [[MTLVertexDescriptor alloc] init];
    vertexDesc.attributes[0].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[0].offset = 0;
    vertexDesc.attributes[0].bufferIndex = 0;
    vertexDesc.attributes[1].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[1].offset = 12;
    vertexDesc.attributes[1].bufferIndex = 0;
    vertexDesc.attributes[2].format = MTLVertexFormatUChar4Normalized;
    vertexDesc.attributes[2].offset = 24;
    vertexDesc.attributes[2].bufferIndex = 0;
    vertexDesc.attributes[3].format = MTLVertexFormatFloat2;
    vertexDesc.attributes[3].offset = 28;
    vertexDesc.attributes[3].bufferIndex = 0;
    vertexDesc.layouts[0].stride = sizeof(vbVertex_t);
    vertexDesc.layouts[0].stepRate = 1;
    vertexDesc.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    descriptor.vertexDescriptor = vertexDesc;

    NSError *error = nil;
    _pipelineState = [_device newRenderPipelineStateWithDescriptor:descriptor
                                                            error:&error];
    if (!_pipelineState) {
        Log::Error("Metal: failed to create pipeline state: %s",
                   [[error localizedDescription] UTF8String]);
        return false;
    }

    _uniformBuffer = [_device newBufferWithLength:sizeof(MetalUniforms)
                                         options:MTLResourceStorageModeShared];
    return true;
}

void nsMetalProgram::Unload() {
    _pipelineState = nil;
    _uniformBuffer = nil;
    _vertexFunction = nil;
    _fragmentFunction = nil;
}

void nsMetalProgram::SetProjView(const float *matrix) {
    fprintf(stderr, "METAL SetProjView: _11=%.4f\n", matrix[0]);
    memcpy(_uniforms.projView, matrix, sizeof(float) * 16);
}

void nsMetalProgram::SetModel(const float *matrix) {
    fprintf(stderr, "METAL SetModel: _11=%.4f _12=%.4f\n", matrix[0], matrix[1]);
    memcpy(_uniforms.model, matrix, sizeof(float) * 16);
}

void nsMetalProgram::SetTextureMatrix(const float *matrix) {
    memcpy(_uniforms.texMat, matrix, sizeof(float) * 16);
}

void nsMetalProgram::SetAlphaCutoff(float cutoff) {
    _uniforms.alphaCutoff = cutoff;
}

void nsMetalProgram::SetHasTexture(bool hasTexture) {
    _uniforms.hasTexture = hasTexture ? 1 : 0;
}

bool nsMetalProgram::Bind(id<MTLRenderCommandEncoder> encoder) {
    if (!_pipelineState) return false;
    [encoder setRenderPipelineState:_pipelineState];
    return true;
}

void nsMetalProgram::UploadUniforms(id<MTLRenderCommandEncoder> encoder) {
    if (!_uniformBuffer) return;
    fprintf(stderr, "METAL Upload: model._11=%.4f model._12=%.4f proj._11=%.4f\n",
        _uniforms.model[0], _uniforms.model[1], _uniforms.projView[0]);

    static int testFrame = 300;
    if (testFrame > 0) {
        for (int i = 0; i < 16; i++) _uniforms.model[i] *= 2.0f;
        fprintf(stderr, "METAL TEST: model x2, model._11=%.4f frames left=%d\n", _uniforms.model[0], testFrame);
        testFrame--;
    }

    memcpy([_uniformBuffer contents], &_uniforms, sizeof(MetalUniforms));
    [encoder setVertexBuffer:_uniformBuffer offset:0 atIndex:1];
    [encoder setFragmentBuffer:_uniformBuffer offset:0 atIndex:1];
}
