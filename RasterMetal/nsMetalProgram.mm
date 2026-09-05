//
// Created by OpenCode
//

#include "nsMetalProgram.h"
#include "nsMetalShaderLibrary.h"
#include "Core/Memory.h"
#include "Engine/RenDevice.h"
#include "nsLib/log.h"
#include <cstddef>
#include <cstring>
#include <string>

nsMetalProgram::nsMetalProgram(id<MTLDevice> device) : _device(device) {
    _shaderLibrary = new nsMetalShaderLibrary(device);
    memset(&_uniforms, 0, sizeof(_uniforms));
    _uniforms.color[0] = 1.0f;
    _uniforms.color[1] = 1.0f;
    _uniforms.color[2] = 1.0f;
    _uniforms.color[3] = 1.0f;
}

nsMetalProgram::~nsMetalProgram() {
    for (auto &it : _userUniforms) {
        delete static_cast<nsMetalUniform *>(it.second);
    }
    _userUniforms.clear();

    Unload();
    delete _shaderLibrary;
}

bool nsMetalProgram::Load(const char *vertexShaderPath, const char *fragmentShaderPath) {
    _vertexShaderPath = vertexShaderPath ? vertexShaderPath : "";
    _fragmentShaderPath = fragmentShaderPath ? fragmentShaderPath : "";
    Invalidate();

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
    descriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
    descriptor.stencilAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
    descriptor.colorAttachments[0].blendingEnabled = YES;
    descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
    descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;

    auto vertexDesc = [[MTLVertexDescriptor alloc] init];
    vertexDesc.attributes[0].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[0].offset = offsetof(vbVertex_t, v);
    vertexDesc.attributes[0].bufferIndex = 0;
    vertexDesc.attributes[1].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[1].offset = offsetof(vbVertex_t, n);
    vertexDesc.attributes[1].bufferIndex = 0;
    vertexDesc.attributes[2].format = MTLVertexFormatUChar4Normalized;
    vertexDesc.attributes[2].offset = offsetof(vbVertex_t, c);
    vertexDesc.attributes[2].bufferIndex = 0;
    vertexDesc.attributes[3].format = MTLVertexFormatFloat2;
    vertexDesc.attributes[3].offset = offsetof(vbVertex_t, tu);
    vertexDesc.attributes[3].bufferIndex = 0;
    vertexDesc.layouts[0].stride = sizeof(vbVertex_t);
    vertexDesc.layouts[0].stepRate = 1;
    vertexDesc.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    descriptor.vertexDescriptor = vertexDesc;

    NSError *error = nil;
    MTLRenderPipelineReflection *reflection = nil;
    _pipelineState = [_device newRenderPipelineStateWithDescriptor:descriptor
                                                          options:MTLPipelineOptionArgumentInfo
                                                       reflection:&reflection
                                                            error:&error];
    if (!_pipelineState) {
        Log::Error("Metal: failed to create pipeline state: %s",
                   [[error localizedDescription] UTF8String]);
        return false;
    }

    _uniformOffsets.clear();
    if (reflection) {
        auto extractOffsets = [this](NSArray<MTLArgument *> *arguments) {
            for (MTLArgument *arg in arguments) {
                if (arg.type == MTLArgumentTypeBuffer && arg.index == 1 && arg.bufferStructType) {
                    for (MTLStructMember *member in arg.bufferStructType.members) {
                        _uniformOffsets[member.name.UTF8String] = member.offset;
                    }
                    return true;
                }
            }
            return false;
        };
        if (!extractOffsets(reflection.fragmentArguments)) {
            extractOffsets(reflection.vertexArguments);
        }
    }

    ResolveUserUniforms();

    return true;
}

void nsMetalProgram::Unload() {
    Invalidate();
    _vertexShaderPath.clear();
    _fragmentShaderPath.clear();
}

void nsMetalProgram::Invalidate() {
    _pipelineState = nil;
	for (uint i = 0; i < kMetalInFlightFrameSlots; ++i) {
	    _uniformBuffers[i].clear();
	}
    _lastFrameIndex = ~0u;
    _uniformSlot = 0;
    _vertexFunction = nil;
    _fragmentFunction = nil;
}

bool nsMetalProgram::EnsureLoaded() {
    if (_vertexFunction && _fragmentFunction) {
        return true;
    }
    if (_vertexShaderPath.empty() || _fragmentShaderPath.empty()) {
        return false;
    }

    const std::string vs = _vertexShaderPath;
    const std::string fs = _fragmentShaderPath;
    return Load(vs.c_str(), fs.c_str());
}

id<MTLRenderPipelineState> nsMetalProgram::GetPipelineState() {
    EnsureLoaded();
    return _pipelineState;
}

id<MTLFunction> nsMetalProgram::GetVertexFunction() {
    EnsureLoaded();
    return _vertexFunction;
}

id<MTLFunction> nsMetalProgram::GetFragmentFunction() {
    EnsureLoaded();
    return _fragmentFunction;
}

void nsMetalProgram::SetProjView(const float *matrix) {
    memcpy(_uniforms.projView, matrix, sizeof(float) * 16);
}

void nsMetalProgram::SetModel(const float *matrix) {
    memcpy(_uniforms.model, matrix, sizeof(float) * 16);
}

void nsMetalProgram::SetTextureMatrix(const float *matrix) {
    memcpy(_uniforms.texMat, matrix, sizeof(float) * 16);
}

void nsMetalProgram::SetColor(const float *color) {
    memcpy(_uniforms.color, color, sizeof(float) * 4);
}

void nsMetalProgram::SetAlphaCutoff(float cutoff) {
    _uniforms.alphaCutoff = cutoff;
}

void nsMetalProgram::SetHasTexture(bool hasTexture) {
    _uniforms.hasTexture = hasTexture ? 1 : 0;
}

void nsMetalProgram::SetHasVertexColor(bool hasVertexColor) {
    _uniforms.hasVertexColor = hasVertexColor ? 1 : 0;
}

bool nsMetalProgram::Bind(id<MTLRenderCommandEncoder> encoder) {
    EnsureLoaded();
    if (!_pipelineState) return false;
    [encoder setRenderPipelineState:_pipelineState];
    return true;
}

bool nsMetalProgram::EnsureUniformBuffer(uint frameSlot, uint uniformSlot) {
	auto &buffers = _uniformBuffers[frameSlot];
	while (buffers.size() <= uniformSlot) {
	    nsMemoryLoopAllocScope metalAllocScope;
	    id<MTLBuffer> buffer = [_device newBufferWithLength:sizeof(MetalUniforms)
	                                                options:MTLResourceStorageModeShared];
        if (!buffer) return false;
        buffers.push_back(buffer);
    }
    return true;
}

void nsMetalProgram::UploadUniforms(id<MTLRenderCommandEncoder> encoder, uint frameIndex) {
    if (_lastFrameIndex != frameIndex) {
        _lastFrameIndex = frameIndex;
        _uniformSlot = 0;
    }

	const auto frameSlot = frameIndex % kMetalInFlightFrameSlots;
    const auto uniformSlot = _uniformSlot++;
    if (!EnsureUniformBuffer(frameSlot, uniformSlot)) return;

    id<MTLBuffer> buffer = _uniformBuffers[frameSlot][uniformSlot];
    memcpy([buffer contents], &_uniforms, sizeof(MetalUniforms));
    [encoder setVertexBuffer:buffer offset:0 atIndex:1];
    [encoder setFragmentBuffer:buffer offset:0 atIndex:1];
}

IShaderUniform *nsMetalProgram::GetUniform(const char *name) {
    auto it = _userUniforms.find(name);
    if (it != _userUniforms.end()) {
        return it->second;
    }

    auto uniform = new nsMetalUniform(this, name);
    _userUniforms[name] = uniform;
    return uniform;
}

bool nsMetalProgram::FindUniformOffset(const char *name, size_t &offset) {
    if (!EnsureLoaded()) return false;
    auto it = _uniformOffsets.find(name);
    if (it == _uniformOffsets.end()) return false;
    offset = it->second;
    return true;
}

void nsMetalProgram::ResolveUserUniforms() {
    for (auto &it : _userUniforms) {
        static_cast<nsMetalUniform *>(it.second)->ResolveAndApply();
    }
}

nsMetalUniform::nsMetalUniform(nsMetalProgram *program, const char *name)
    : _program(program), _name(name) {
}

void nsMetalUniform::SetFloat(float value) {
    Set(&value, 1);
}

void nsMetalUniform::SetFloat3(const float value[3]) {
    Set(value, 3);
}

void nsMetalUniform::SetFloat4(const float value[4]) {
    Set(value, 4);
}

void nsMetalUniform::Set(const float *value, int count) {
    memcpy(_value, value, sizeof(float) * count);
    _count = count;

    if (!_fieldPtr) {
        ResolveAndApply();
        return;
    }

    if (_fieldPtr) {
        memcpy(_fieldPtr, value, sizeof(float) * count);
    }
}

void nsMetalUniform::ResolveAndApply() {
    size_t offset = 0;
    _fieldPtr = nullptr;
    if (!_program->FindUniformOffset(_name.c_str(), offset) ||
        offset > sizeof(MetalUniforms) - sizeof(_value) || offset % alignof(float) != 0) {
        return;
    }
    _fieldPtr = reinterpret_cast<float *>(reinterpret_cast<char *>(_program->GetUniforms()) + offset);
    if (_count > 0) {
        memcpy(_fieldPtr, _value, sizeof(float) * _count);
    }
}
