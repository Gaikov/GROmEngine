//
// Created by OpenCode
//

#include "nsMetalRenderState.h"
#include "Core/ParseFile.h"
#include "nsLib/log.h"
#include <cstring>

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

bool nsMetalRenderState::Load(const char *fileName) {
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
    auto vs = ps_get_str(ss, "vs", nsMetalProgramsCache::DEFAULT_VERTEX_SHADER);
    auto fs = ps_get_str(ss, "fs", nsMetalProgramsCache::DEFAULT_FRAGMENT_SHADER);
    _program = _programs.GetProgram(vs, fs);
    if (!_program) return false;

    _zEnable  = ps_get_f(ss, "z_enable", 1) != 0;
    _zWrite   = ps_get_f(ss, "z_write", 1) != 0;

    _alphaTest = ps_get_f(ss, "alpha_test", 0) != 0;
    _alphaRef  = ps_get_f(ss, "alpha_ref", 0);
    if (_alphaRef > 255) _alphaRef = 255;
    _alphaRef /= 255.0f;

    auto src = ps_get_str(ss, "blend_src", "none");
    auto dst = ps_get_str(ss, "blend_dst", "none");
    _srcBlend = MapBlendFactor(src, MTLBlendFactorOne);
    _dstBlend = MapBlendFactor(dst, MTLBlendFactorZero);
    _alphaBlend = !StrEqual(src, "none") && !StrEqual(dst, "none");

    _cullMode = ps_get_f(ss, "cull_mode", 1) != 0;

    return CreatePipeline();
}

bool nsMetalRenderState::CreatePipeline() {
    auto vertexFunc = _program->GetVertexFunction();
    auto fragmentFunc = _program->GetFragmentFunction();
    if (!vertexFunc || !fragmentFunc) return false;

    auto desc = [[MTLRenderPipelineDescriptor alloc] init];
    desc.vertexFunction = vertexFunc;
    desc.fragmentFunction = fragmentFunc;
    desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

    if (_alphaBlend) {
        desc.colorAttachments[0].blendingEnabled = YES;
        desc.colorAttachments[0].sourceRGBBlendFactor = _srcBlend;
        desc.colorAttachments[0].destinationRGBBlendFactor = _dstBlend;
        desc.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
        desc.colorAttachments[0].sourceAlphaBlendFactor = _srcBlend;
        desc.colorAttachments[0].destinationAlphaBlendFactor = _dstBlend;
        desc.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
    }

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
    desc.vertexDescriptor = vertexDesc;

    NSError *error = nil;
    _pipelineState = [_device newRenderPipelineStateWithDescriptor:desc error:&error];
    if (!_pipelineState) {
        Log::Error("Metal: failed to create pipeline state: %s",
                   [[error localizedDescription] UTF8String]);
        return false;
    }

    MTLDepthStencilDescriptor *dsDesc = [[MTLDepthStencilDescriptor alloc] init];
    dsDesc.depthCompareFunction = _zEnable ? MTLCompareFunctionLessEqual : MTLCompareFunctionAlways;
    dsDesc.depthWriteEnabled = _zEnable && _zWrite;
    _depthStencilState = [_device newDepthStencilStateWithDescriptor:dsDesc];

    return true;
}

void nsMetalRenderState::Apply(id<MTLRenderCommandEncoder> encoder,
                                nsMetalProgramsCache &programs,
                                nsMetalRenderState *prev) {
    if (!prev) {
        [encoder setRenderPipelineState:_pipelineState];
        if (_depthStencilState) [encoder setDepthStencilState:_depthStencilState];
        programs.Bind(_program, true);
        return;
    }

    if (_pipelineState != prev->_pipelineState) {
        [encoder setRenderPipelineState:_pipelineState];
    }
    if (_depthStencilState != prev->_depthStencilState) {
        [encoder setDepthStencilState:_depthStencilState];
    }
    if (_program != prev->_program) {
        programs.Bind(_program, true);
    }
}
