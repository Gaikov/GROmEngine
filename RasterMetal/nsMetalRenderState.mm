//
// Created by OpenCode
//

#include "nsMetalRenderState.h"
#include "Core/ParseFile.h"
#include "nsLib/log.h"
#include <cstddef>
#include <cstring>
#include <string>

static MTLBlendFactor MapBlendFactor(const char *name, MTLBlendFactor def) {
    if (!name || !*name || StrEqual(name, "none")) return def;
    if (StrEqual(name, "zero"))              return MTLBlendFactorZero;
    if (StrEqual(name, "one"))               return MTLBlendFactorOne;
    if (StrEqual(name, "src_color"))         return MTLBlendFactorSourceColor;
    if (StrEqual(name, "inv_src_color"))     return MTLBlendFactorOneMinusSourceColor;
    if (StrEqual(name, "src_alpha"))         return MTLBlendFactorSourceAlpha;
    if (StrEqual(name, "inv_src_alpha"))     return MTLBlendFactorOneMinusSourceAlpha;
    if (StrEqual(name, "dst_alpha"))         return MTLBlendFactorDestinationAlpha;
    if (StrEqual(name, "inv_dst_alpha"))     return MTLBlendFactorOneMinusDestinationAlpha;
    if (StrEqual(name, "dst_color"))         return MTLBlendFactorDestinationColor;
    if (StrEqual(name, "inv_dst_color"))     return MTLBlendFactorOneMinusDestinationColor;
    if (StrEqual(name, "src_alpha_sat"))     return MTLBlendFactorSourceAlphaSaturated;
    return def;
}

nsMetalRenderState::nsMetalRenderState(id<MTLDevice> device, nsMetalProgramsCache &programs)
    : _device(device), _programs(programs) {}

bool nsMetalRenderState::InitDefault() {
    Invalidate();
    _fileName = "";
    _program = _programs.GetProgram(nsMetalProgramsCache::DEFAULT_VERTEX_SHADER,
                                    nsMetalProgramsCache::DEFAULT_FRAGMENT_SHADER);
    return _program && EnsureResources();
}

bool nsMetalRenderState::Load(const char *fileName) {
    Invalidate();
    _fileName = fileName;
    Log::Info("...loading Metal render state: %s", fileName);
    nsParseFile pf;
    auto ss = pf.BeginFile(fileName);
    if (!ss) {
        Log::Warning("can't load render state '%s'", fileName);
        return false;
    }
    return Parse(ss);
}

bool nsMetalRenderState::Parse(script_state_t *ss) {
    const std::string vs = ps_get_str(ss, "vs", nsMetalProgramsCache::DEFAULT_VERTEX_SHADER);
    const std::string fs = ps_get_str(ss, "fs", nsMetalProgramsCache::DEFAULT_FRAGMENT_SHADER);
    _program = _programs.GetProgram(
        nsMetalProgramsCache::ResolveShaderPath(vs, true).c_str(),
        nsMetalProgramsCache::ResolveShaderPath(fs, false).c_str());
    if (!_program) return false;

    _zEnable  = ps_get_f(ss, "z_enable", 1) != 0;
    _zWrite   = ps_get_f(ss, "z_write", 1) != 0;

    _alphaTest = ps_get_f(ss, "alpha_test", 0) != 0;
    _alphaRef  = ps_get_f(ss, "alpha_ref", 0);
    if (_alphaRef > 255) _alphaRef = 255;
    _alphaRef /= 255.0f;

    const std::string src = ps_get_str(ss, "blend_src", "none");
    const std::string dst = ps_get_str(ss, "blend_dst", "none");
    const std::string srcAlpha = ps_get_str(ss, "blend_src_alpha", "none");
    const std::string dstAlpha = ps_get_str(ss, "blend_dst_alpha", "none");
    _srcBlend = MapBlendFactor(src.c_str(), MTLBlendFactorOne);
    _dstBlend = MapBlendFactor(dst.c_str(), MTLBlendFactorZero);
    _srcAlphaBlend = MapBlendFactor(srcAlpha.c_str(), _srcBlend);
    _dstAlphaBlend = MapBlendFactor(dstAlpha.c_str(), _dstBlend);
    _alphaBlend = !StrEqual(src.c_str(), "none")
        || !StrEqual(dst.c_str(), "none")
        || !StrEqual(srcAlpha.c_str(), "none")
        || !StrEqual(dstAlpha.c_str(), "none");

    _cullMode = ps_get_f(ss, "cull_mode", 1) != 0;
    _texCoordU = StrEqual(ps_get_str(ss, "tex_coord_u", "wrap"), "clamp")
        ? MTLSamplerAddressModeClampToEdge
        : MTLSamplerAddressModeRepeat;
    _texCoordV = StrEqual(ps_get_str(ss, "tex_coord_v", "wrap"), "clamp")
        ? MTLSamplerAddressModeClampToEdge
        : MTLSamplerAddressModeRepeat;

    return EnsureResources();
}

bool nsMetalRenderState::CreatePipelineState(MTLColorWriteMask mask) {
    if (!_program) return false;
    const auto index = static_cast<size_t>(mask) & (COLOR_WRITE_MASK_VARIANTS - 1);
    if (_pipelineStates[index]) return true;

    auto vertexFunc = _program->GetVertexFunction();
    auto fragmentFunc = _program->GetFragmentFunction();
    if (!vertexFunc || !fragmentFunc) return false;

    auto desc = [[MTLRenderPipelineDescriptor alloc] init];
    desc.vertexFunction = vertexFunc;
    desc.fragmentFunction = fragmentFunc;
    desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    desc.colorAttachments[0].writeMask = mask;
    desc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
    desc.stencilAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;

    if (_alphaBlend) {
        desc.colorAttachments[0].blendingEnabled = YES;
        desc.colorAttachments[0].sourceRGBBlendFactor = _srcBlend;
        desc.colorAttachments[0].destinationRGBBlendFactor = _dstBlend;
        desc.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
        desc.colorAttachments[0].sourceAlphaBlendFactor = _srcAlphaBlend;
        desc.colorAttachments[0].destinationAlphaBlendFactor = _dstAlphaBlend;
        desc.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
    }

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
    desc.vertexDescriptor = vertexDesc;

    NSError *error = nil;
    _pipelineStates[index] = [_device newRenderPipelineStateWithDescriptor:desc error:&error];
    if (!_pipelineStates[index]) {
        Log::Error("Metal: failed to create pipeline state: %s",
                   [[error localizedDescription] UTF8String]);
        return false;
    }

    return true;
}

bool nsMetalRenderState::CreateCommonStates() {
    if (!_depthStencilState) {
        MTLDepthStencilDescriptor *dsDesc = [[MTLDepthStencilDescriptor alloc] init];
        dsDesc.depthCompareFunction = _zEnable ? MTLCompareFunctionLessEqual : MTLCompareFunctionAlways;
        dsDesc.depthWriteEnabled = _zEnable && _zWrite;
        _depthStencilState = [_device newDepthStencilStateWithDescriptor:dsDesc];
        if (!_depthStencilState) return false;
    }

    if (!_samplerState) {
        MTLSamplerDescriptor *samplerDesc = [MTLSamplerDescriptor new];
        samplerDesc.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.magFilter = MTLSamplerMinMagFilterLinear;
        samplerDesc.sAddressMode = _texCoordU;
        samplerDesc.tAddressMode = _texCoordV;
        _samplerState = [_device newSamplerStateWithDescriptor:samplerDesc];
        if (!_samplerState) return false;
    }

    return true;
}

bool nsMetalRenderState::EnsureResources() {
    return CreatePipelineState(_colorWriteMask) && CreateCommonStates();
}

id<MTLRenderPipelineState> nsMetalRenderState::GetPipelineState() const {
    const auto index = static_cast<size_t>(_colorWriteMask) & (COLOR_WRITE_MASK_VARIANTS - 1);
    return _pipelineStates[index];
}

IShaderUniform *nsMetalRenderState::GetUniform(const char *name) {
    return _program ? _program->GetUniform(name) : nullptr;
}

void nsMetalRenderState::Invalidate() {
    _pipelineStates.fill(nil);
    _depthStencilState = nil;
    _samplerState = nil;
}

void nsMetalRenderState::SetColorWriteMask(MTLColorWriteMask mask) {
    _colorWriteMask = mask;
}

void nsMetalRenderState::Apply(id<MTLRenderCommandEncoder> encoder,
                                nsMetalProgramsCache &programs,
                                nsMetalRenderState *prev) {
    if (!EnsureResources()) {
        return;
    }

    const auto pipelineState = GetPipelineState();
    if (!pipelineState) {
        return;
    }
    if (!prev || !prev->GetPipelineState() ||
        !prev->_depthStencilState || !prev->_samplerState) {
        prev = nullptr;
    }

    if (!prev) {
        [encoder setRenderPipelineState:pipelineState];
        if (_depthStencilState) [encoder setDepthStencilState:_depthStencilState];
        [encoder setCullMode:_cullMode ? MTLCullModeBack : MTLCullModeNone];
        if (_samplerState) [encoder setFragmentSamplerState:_samplerState atIndex:0];
        programs.Bind(_program, true);
        return;
    }

    if (pipelineState != prev->GetPipelineState()) {
        [encoder setRenderPipelineState:pipelineState];
    }
    if (_depthStencilState != prev->_depthStencilState) {
        [encoder setDepthStencilState:_depthStencilState];
    }
    if (_cullMode != prev->_cullMode) {
        [encoder setCullMode:_cullMode ? MTLCullModeBack : MTLCullModeNone];
    }
    if (_samplerState != prev->_samplerState) {
        [encoder setFragmentSamplerState:_samplerState atIndex:0];
    }
    if (_program != prev->_program) {
        programs.Bind(_program, true);
    }
}
