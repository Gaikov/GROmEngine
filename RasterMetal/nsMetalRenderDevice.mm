//
// Created by OpenCode
//

#include "nsMetalRenderDevice.h"
#include "nsMetalRenderState.h"
#include "nsMetalStencilState.h"
#include "Core/Config.h"
#include "Core/Memory.h"
#include "nsLib/log.h"
#include "nsLib/FilePath.h"
#include "Engine/Platform.h"
#import <AppKit/AppKit.h>
#import <QuartzCore/CAMetalLayer.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
extern "C" id glfwGetCocoaWindow(GLFWwindow* window);

void DestroyMetalRenderDevice(nsMetalRenderDevice *device);

@interface nsMetalView : MTKView
@end

@implementation nsMetalView
- (void)drawRect:(NSRect)dirtyRect {
    // Never draw automatically — fully manual rendering
}
@end

nsMetalRenderDevice::nsMetalRenderDevice()
    : _projMatrix(1), _viewMatrix(1) {}

nsMetalRenderDevice::~nsMetalRenderDevice() = default;

bool nsMetalRenderDevice::Init(void *wnd) {
    Log::Info("*****************************************");
    Log::Info("+     initializing Metal renderer       +");
    Log::Info("*****************************************");

    _device = MTLCreateSystemDefaultDevice();
    if (!_device) {
        Log::Error("Metal is not supported on this device!");
        return false;
    }

	Log::Info("Metal device: %s", [[_device name] UTF8String]);

	g_cfg->RegCmd("r_restart", [this](int argc, const char* argv[]) { _queryRestart = true; });
	r_vsync->AddHandler(nsVar::NSVAR_CHANGED, [this](const nsBaseEvent*) {
	    ApplyVSync();
	});

	if (!_displayModes.Init()) {
	    return false;
	}
	if (!ApplyDisplayMode()) {
	    return false;
	}

	_commandQueue = [_device newCommandQueue];
	_passDescriptor = [[MTLRenderPassDescriptor alloc] init];

    void *wndPtr = App_GetPlatform()->GetWindowHandler();
    if (wndPtr) {
        GLFWwindow *glfwWin = (GLFWwindow *)wndPtr;
        NSWindow *window = glfwGetCocoaWindow(glfwWin);
        if (window) {
            NSView *view = [window contentView];
            view.wantsLayer = YES;
            _mtkView = [[nsMetalView alloc] initWithFrame:view.bounds device:_device];
            _mtkView.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
            _mtkView.framebufferOnly = NO;
            _mtkView.layer.opaque = YES;
            _mtkView.enableSetNeedsDisplay = NO;
            _mtkView.paused = YES;
            _mtkView.autoResizeDrawable = NO;
            _mtkView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
            auto layer = (CAMetalLayer *)_mtkView.layer;
            layer.device = _device;
            layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
            layer.framebufferOnly = NO;
            layer.opaque = YES;
            ApplyVSync();
            [view addSubview:_mtkView positioned:NSWindowAbove relativeTo:nil];
        }
    }

    _textures = new nsMetalTexturesCache(_device);
    _programs = new nsMetalProgramsCache(_device);
    if (!_programs->Init()) {
        return false;
    }

    _defaultState = new nsMetalRenderState(_device, *_programs);
    if (!_defaultState->InitDefault()) {
        return false;
    }
    StateApply(_defaultState);

    _quadBuff = new nsMetalVertexBuffer(_device, _textures, 4, 6, false);
    auto i = _quadBuff->GetWriteIndices();
    i[0] = 0; i[1] = 1; i[2] = 2;
    i[3] = 0; i[4] = 2; i[5] = 3;

    return true;
}

void nsMetalRenderDevice::Release() {
    EndEncoder();
    _commandBuffer = nil;
    _currentDrawable = nil;
    _depthStencilTexture = nil;
    _passDescriptor = nil;

    _currentState = nullptr;
    _currentStencil = nullptr;
    for (auto &it : _stateCache) delete it.second;
    _stateCache.clear();
    _stateRefs.clear();
    delete _defaultState; _defaultState = nullptr;

    delete _quadBuff; _quadBuff = nullptr;

	for (auto vb : _allocatedVBS) delete vb;
	_allocatedVBS.clear();
	for (auto rt : _allocatedRenderTextures) delete rt;
	_allocatedRenderTextures.clear();

    delete _programs; _programs = nullptr;
    delete _textures; _textures = nullptr;

    _commandQueue = nil;
    [_mtkView removeFromSuperview];
    _mtkView = nil;
    _device = nil;

    DestroyMetalRenderDevice(this);
}

void nsMetalRenderDevice::InvalidateResources() {
	_queryRestart = true;
}

void nsMetalRenderDevice::RestartResources() {
	ApplyDisplayMode();
	_currentState = nullptr;
	if (_defaultState) {
	    _defaultState->Invalidate();
	}
	for (auto &it : _stateCache) {
	    it.second->Invalidate();
	}
    _textures->UnloadFromGPU();
    _programs->Invalidate();
	for (auto vb : _allocatedVBS) {
	    vb->Invalidate();
	}
	for (auto rt : _allocatedRenderTextures) {
	    rt->Invalidate();
	}
}
void nsMetalRenderDevice::GetDisplayInfo(DisplayInfo &info) {
    auto &modes = _displayModes.GetModes();
    for (auto &mode : modes) {
        info.modes.push_back({mode.width, mode.height});
    }
}
const rasterConfig_t* nsMetalRenderDevice::GetCurrentConfig() {
    return &_displayModes.GetConfig();
}
void nsMetalRenderDevice::SetColor(const float c[4]) {
    _currentColor = nsColor(c[0], c[1], c[2], c[3]);
}

bool nsMetalRenderDevice::BeginEncoder() {
    if (!_mtkView || !_mtkView.window || _mtkView.bounds.size.width <= 0) {
        return false;
    }

    CGFloat scale = _mtkView.window.backingScaleFactor;
    CGSize drawableSize = CGSizeMake(_mtkView.bounds.size.width * scale,
                                      _mtkView.bounds.size.height * scale);
    if (drawableSize.width != _mtkView.drawableSize.width ||
        drawableSize.height != _mtkView.drawableSize.height) {
        _mtkView.drawableSize = drawableSize;
    }

    auto layer = (CAMetalLayer *)_mtkView.layer;
    layer.drawableSize = drawableSize;

	{
	    nsMemoryLoopAllocScope metalAllocScope;
	    _commandBuffer = [_commandQueue commandBuffer];
	    _currentDrawable = [layer nextDrawable];
	}
    if (!_currentDrawable || _currentDrawable.texture.width == 0 || _currentDrawable.texture.height == 0) {
        _currentDrawable = nil;
        _commandBuffer = nil;
        return false;
    }

	EnsureDepthStencilTexture((int)_currentDrawable.texture.width,
	                          (int)_currentDrawable.texture.height);

	MTLRenderPassDescriptor *passDesc = CreatePassDescriptor(_currentDrawable.texture, _depthStencilTexture);
	{
	    nsMemoryLoopAllocScope metalAllocScope;
	    _encoder = [_commandBuffer renderCommandEncoderWithDescriptor:passDesc];
	}
    if (!_encoder) return false;

    MTLViewport viewport = {
        0.0, 0.0,
        (double)_currentDrawable.texture.width,
        (double)_currentDrawable.texture.height,
        0.0, 1.0
    };
    [_encoder setViewport:viewport];

    return true;
}

void nsMetalRenderDevice::EndEncoder() {
    if (_encoder) { [_encoder endEncoding]; _encoder = nil; }
}

bool nsMetalRenderDevice::ApplyDisplayMode() {
    EndEncoder();
    _currentDrawable = nil;
    _commandBuffer = nil;
    _depthStencilTexture = nil;
    const bool result = _displayModes.ApplyCurrentMode();
    ApplyVSync();
    return result;
}

void nsMetalRenderDevice::ApplyVSync() {
    if (!_mtkView || !_mtkView.layer) {
        return;
    }

    auto layer = (CAMetalLayer *)_mtkView.layer;
    layer.displaySyncEnabled = r_vsync->Bool() ? YES : NO;
}

bool nsMetalRenderDevice::BeginScene() {
	_currentRenderTexture = nullptr;
	_frameIndex++;
	if (_queryRestart) {
	    _queryRestart = false;
	    RestartResources();
	}
	return BeginEncoder();
}

void nsMetalRenderDevice::EndScene() {
    EndEncoder();
    if (_commandBuffer && _currentDrawable) {
        [_commandBuffer presentDrawable:_currentDrawable];
        [_commandBuffer commit];
        _currentDrawable = nil;
        _commandBuffer = nil;
    }
}

void nsMetalRenderDevice::ClearScene(uint flags) {
    _pendingClearFlags |= flags;
    if (!_encoder) {
        return;
    }

    EndEncoder();
    id<MTLTexture> color = nil;
    id<MTLTexture> depthStencil = nil;
    if (_currentRenderTexture) {
        color = _currentRenderTexture->GetColorTexture();
        depthStencil = _currentRenderTexture->GetDepthStencilTexture();
    } else if (_currentDrawable) {
        color = _currentDrawable.texture;
        depthStencil = _depthStencilTexture;
    }
    if (!color) {
        return;
    }

	MTLRenderPassDescriptor *passDesc = CreatePassDescriptor(color, depthStencil);
	{
	    nsMemoryLoopAllocScope metalAllocScope;
	    _encoder = [_commandBuffer renderCommandEncoderWithDescriptor:passDesc];
	}
    MTLViewport viewport = {0.0, 0.0, (double)color.width, (double)color.height, 0.0, 1.0};
    [_encoder setViewport:viewport];
    if (_currentState) {
        _currentState->Apply(_encoder, *_programs, nullptr);
        if (_currentStencil) {
            _currentStencil->Apply(_encoder,
                                   _currentState->IsDepthEnabled(),
                                   _currentState->IsDepthWriteEnabled());
        }
    }
}

ITexture* nsMetalRenderDevice::TextureLoad(const char *filename, bool, texfmt_t, int flags) {
    return _textures->GetTexture(filename, flags);
}

const char* nsMetalRenderDevice::TextureGetPath(ITexture *t) {
    auto tex = dynamic_cast<nsMetalTexture*>(t);
    return tex ? tex->GetPath() : nullptr;
}
ITexture* nsMetalRenderDevice::TextureGenerate(int width, int height, const void *data, texfmt_t, bool) {
    auto bmData = nsBitmapData::Create(width, height);
    bmData->SetData(data);
    return _textures->CreateFromBitmapData(bmData);
}
void nsMetalRenderDevice::TextureRelease(ITexture *texture) {
    _textures->ReleaseResource(dynamic_cast<nsMetalTexture*>(texture));
}

void nsMetalRenderDevice::TextureBind(ITexture *texture) {
    if (!_encoder) return;
    auto tex = dynamic_cast<nsMetalTexture*>(texture);
    if (tex) {
        _textures->BindTexture(tex, _encoder);
        _programs->SetTextureBound(true);
        return;
    }
    auto renderTexture = dynamic_cast<nsMetalRenderTexture*>(texture);
    if (renderTexture) {
        renderTexture->Bind(_encoder, 0);
        _programs->SetTextureBound(true);
        return;
    }
    _textures->BindTexture(nullptr, _encoder);
    _programs->SetTextureBound(false);
}

void nsMetalRenderDevice::TextureTranform(const float *offs2, const float *scale2) {
    nsMatrix m;
    m.Identity();
    if (offs2) m.SetPos(offs2);
    if (scale2) m.Scale(scale2[0], scale2[1], 1.0f);
    _programs->SetTextureMatrix(m);
}

IRenState* nsMetalRenderDevice::StateLoad(const char *fileName) {
    if (!fileName || !*fileName) return nullptr;
    auto it = _stateCache.find(fileName);
    if (it != _stateCache.end()) {
        _stateRefs[it->second]++;
        return it->second;
    }
    auto state = new nsMetalRenderState(_device, *_programs);
    if (!state->Load(fileName)) {
        delete state;
        return nullptr;
    }
    _stateCache[fileName] = state;
    _stateRefs[state] = 1;
    return state;
}

const char* nsMetalRenderDevice::StateGetPath(IRenState *s) {
    auto state = dynamic_cast<nsMetalRenderState*>(s);
    return state ? state->GetPath() : "";
}

void nsMetalRenderDevice::StateRelease(IRenState *state) {
    auto metalState = dynamic_cast<nsMetalRenderState*>(state);
    if (!metalState || metalState == _defaultState) return;
    auto refIt = _stateRefs.find(metalState);
    if (refIt != _stateRefs.end() && refIt->second > 1) {
        refIt->second--;
        return;
    }
    for (auto it = _stateCache.begin(); it != _stateCache.end(); ++it) {
        if (it->second == metalState) {
            if (_currentState == metalState) {
                _currentState = nullptr;
            }
            _stateRefs.erase(metalState);
            delete metalState;
            _stateCache.erase(it);
            return;
        }
    }
}

void nsMetalRenderDevice::StateApply(IRenState *s) {
    auto state = dynamic_cast<nsMetalRenderState*>(s);
    if (!state) state = _defaultState;
    if (state == _currentState) return;

    if (_encoder) {
        if (_currentStencil) {
            state->SetColorWriteMask(_currentStencil->GetColorWriteMask());
        } else {
            state->SetColorWriteMask(MTLColorWriteMaskAll);
        }
        state->Apply(_encoder, *_programs, _currentState);
        if (_currentStencil) {
            _currentStencil->Apply(_encoder, state->IsDepthEnabled(), state->IsDepthWriteEnabled());
        }
    }
    _currentState = state;

    if (state->IsAlphaTest()) {
        _programs->SetAlphaCutoff(state->GetAlphaCutoff());
    } else {
        _programs->SetAlphaCutoff(0);
    }
}

void nsMetalRenderDevice::ApplyProjectionMatrix() {
    nsMatrix projView = _viewMatrix * _projMatrix;
    _programs->SetProjViewMatrix(projView);
}

void nsMetalRenderDevice::LoadProjMatrix(const float *m) {
    _projMatrix = m;
    ApplyProjectionMatrix();
}
void nsMetalRenderDevice::LoadViewMartix(const float *m) {
    _viewMatrix = m;
    ApplyProjectionMatrix();
}
void nsMetalRenderDevice::LoadMatrix(const float *m) { _programs->SetModelMatrix(m); }

IVertexBuffer* nsMetalRenderDevice::VerticesCreate(uint vertsCount, uint indexCount, bool, bool useColors) {
    auto vb = new nsMetalVertexBuffer(_device, _textures, vertsCount, indexCount, useColors);
    _allocatedVBS.push_back(vb);
    return vb;
}

void nsMetalRenderDevice::VerticesRelease(IVertexBuffer *vb) {
    auto it = std::find(_allocatedVBS.begin(), _allocatedVBS.end(), vb);
    if (it != _allocatedVBS.end()) {
        auto metalVB = *it;
        _allocatedVBS.erase(it);
        delete metalVB;
    }
}

void nsMetalRenderDevice::VerticesDraw(IVertexBuffer *vb) {
    if (!_encoder) return;
    auto state = _currentState ? _currentState : _defaultState;
    if (!state) return;
    auto program = state->GetProgram();
    if (!program) program = _programs->GetDefaultProgram();
    if (!program) return;
    _programs->Bind(program, true);
    state->Apply(_encoder, *_programs, nullptr);
    if (_currentStencil) {
        _currentStencil->Apply(_encoder, state->IsDepthEnabled(), state->IsDepthWriteEnabled());
    }
    auto tex = _textures->GetBoundTexture();
    if (tex) tex->Bind(_encoder, 0);
    auto metalVB = dynamic_cast<nsMetalVertexBuffer*>(vb);
    if (metalVB) {
        metalVB->UseColor(_currentColor);
        program->SetColor(_currentColor);
        program->SetHasTexture(_textures->HasBoundTexture());
        program->SetHasVertexColor(metalVB->UsesColor());
        program->UploadUniforms(_encoder, _frameIndex);
        metalVB->Draw(_encoder, _frameIndex);
    }
}

void nsMetalRenderDevice::DrawLinedSprite(float x1, float y1, float x2, float y2, float width, float height) {
    nsVec2 p1(x1, y1);
    nsVec2 dir = (nsVec2(x2, y2) - p1) * 0.5f;
    p1 += dir;
    dir.Norm();

    nsVec2 down = dir.GetRight();
    p1 -= dir * width * 0.5f;
    p1 -= down * height * 0.5f;

    dir *= width;
    down *= height;

    vbVertex_t spr[4];
    spr[1].SetUV(0, 1);
    spr[2].SetUV(1, 1);
    spr[3].SetUV(1, 0);

    spr[0].v.x = p1.x;
    spr[0].v.y = p1.y;
    nsVec2 tmp = p1 + down;
    spr[1].v.x = tmp.x;
    spr[1].v.y = tmp.y;
    tmp = tmp + dir;
    spr[2].v.x = tmp.x;
    spr[2].v.y = tmp.y;
    tmp = p1 + dir;
    spr[3].v.x = tmp.x;
    spr[3].v.y = tmp.y;

    DrawQuad(spr);
}

void nsMetalRenderDevice::DrawSprite3D(const nsVec3 &pos, float width, float height, float angle) {
    auto x = _viewMatrix.GetViewX();
    auto y = _viewMatrix.GetViewY();

    if (angle != 0.0f) {
        nsMatrix m;
        m.AxisRotate(y.Cross(x), angle);
        y *= m;
        x *= m;
    }

    nsVec3 down = -y * width;
    nsVec3 right = x * height;
    nsVec3 origin = pos - right / 2.0f;
    origin -= down / 2.0f;

    nsVec3 zero;
    auto c = 0xffffffff;
    vbVertex_t v[4] = {
        {origin, zero, c, 0, 0},
        {origin + down, zero, c, 0, 1},
        {origin + down + right, zero, c, 1, 1},
        {origin + right, zero, c, 1, 0}
    };

    LoadMatrix(nsMatrix::identity);
    DrawQuad(v);
}

void nsMetalRenderDevice::DrawChar3D(const rchar_t *ch, const nsVec3 &pos, float sx, float sy) {
    if (!ch) return;

    float w = ch->size[0] * sx;
    float h = ch->size[1] * sy;
    vbVertex_t v[4];
    v[0].tu = ch->coord[0];
    v[0].tv = ch->coord[1] + ch->tex_size[1];
    v[1].tu = ch->coord[0] + ch->tex_size[0];
    v[1].tv = ch->coord[1] + ch->tex_size[1];
    v[2].tu = ch->coord[0] + ch->tex_size[0];
    v[2].tv = ch->coord[1];
    v[3].tu = ch->coord[0];
    v[3].tv = ch->coord[1];

    v[0].v = pos;
    v[1].v = nsVec3(pos.x + w, pos.y, pos.z);
    v[2].v = nsVec3(pos.x + w, pos.y + h, pos.z);
    v[3].v = nsVec3(pos.x, pos.y + h, pos.z);

    DrawQuad(v);
}

void nsMetalRenderDevice::DrawPlaneSprite(const nsVec3 &p1, const nsVec3 &p2, float width, uint wrap_count) {
    nsVec3 dir = -_viewMatrix.GetViewZ();
    nsVec3 l = p2 - p1;
    nsVec3 w = dir.Cross(l);
    w.Norm();
    w *= width / 2.0f;

    vbVertex_t v[4];
    v[0].tu = 0;
    v[0].tv = 0;
    v[1].tu = 0;
    v[1].tv = (float)wrap_count;
    v[2].tu = 1;
    v[2].tv = (float)wrap_count;
    v[3].tu = 1;
    v[3].tv = 0;
    v[0].c = v[1].c = v[2].c = v[3].c = 0xffffffff;
    v[0].v = p2 - w;
    v[1].v = p1 - w;
    v[2].v = p1 + w;
    v[3].v = p2 + w;

    LoadMatrix(nsMatrix::identity);
    DrawQuad(v);
}

IStencilState* nsMetalRenderDevice::StencilLoad(const char *fileName) {
    if (!fileName) return nullptr;

    std::string fallback = fileName;
    const std::string ggss = ".ggss";
    if (fallback.size() >= ggss.size() &&
        fallback.compare(fallback.size() - ggss.size(), ggss.size(), ggss) == 0) {
        fallback.replace(fallback.size() - ggss.size(), ggss.size(), ".ggrs");
        if (!nsFilePath::Exists(fileName) && nsFilePath::Exists(fallback.c_str())) {
            return nsMetalStencilState::Load(_device, fallback.c_str());
        }
    }

    auto state = nsMetalStencilState::Load(_device, fileName);
    if (state) return state;

    if (fallback != fileName) {
        return nsMetalStencilState::Load(_device, fallback.c_str());
    }
    return nullptr;
}
void nsMetalRenderDevice::StencilRelease(IStencilState *state) {
    delete dynamic_cast<nsMetalStencilState*>(state);
}
void nsMetalRenderDevice::StencilApply(IStencilState *state) {
    _currentStencil = dynamic_cast<nsMetalStencilState*>(state);
    auto renState = _currentState ? _currentState : _defaultState;
    if (!renState) return;

    const auto colorMask = _currentStencil
        ? _currentStencil->GetColorWriteMask()
        : MTLColorWriteMaskAll;
    renState->SetColorWriteMask(colorMask);

    if (_encoder) {
        renState->Apply(_encoder, *_programs, nullptr);
        if (_currentStencil) {
            _currentStencil->Apply(_encoder, renState->IsDepthEnabled(), renState->IsDepthWriteEnabled());
        }
    }
}

IRenderTexture* nsMetalRenderDevice::RenderTextureCreate(int width, int height, texfmt_t fmt) {
    auto rt = new nsMetalRenderTexture(_device, width, height, fmt);
    _allocatedRenderTextures.push_back(rt);
    return rt;
}

void nsMetalRenderDevice::RenderTextureBind(IRenderTexture *rt) {
    EndEncoder();
	_currentRenderTexture = dynamic_cast<nsMetalRenderTexture*>(rt);
	if (!_commandBuffer) {
	    nsMemoryLoopAllocScope metalAllocScope;
	    _commandBuffer = [_commandQueue commandBuffer];
	}

    id<MTLTexture> color = nil;
    id<MTLTexture> depthStencil = nil;
    if (_currentRenderTexture) {
        color = _currentRenderTexture->GetColorTexture();
        depthStencil = _currentRenderTexture->GetDepthStencilTexture();
    } else if (_currentDrawable) {
        color = _currentDrawable.texture;
        depthStencil = _depthStencilTexture;
    }
    if (!color) return;

	MTLRenderPassDescriptor *passDesc = CreatePassDescriptor(color, depthStencil);
	{
	    nsMemoryLoopAllocScope metalAllocScope;
	    _encoder = [_commandBuffer renderCommandEncoderWithDescriptor:passDesc];
	}
    MTLViewport viewport = {0.0, 0.0, (double)color.width, (double)color.height, 0.0, 1.0};
    [_encoder setViewport:viewport];
    if (_currentState) {
        _currentState->Apply(_encoder, *_programs, nullptr);
    }
}

bool nsMetalRenderDevice::GetActiveFrameContext(nsMetalFrameContext &context) const {
    context = {};
    if (!_device || !_passDescriptor || !_commandBuffer || !_encoder) {
        return false;
    }

    context.device = _device;
    context.renderPassDescriptor = _passDescriptor;
    context.commandBuffer = _commandBuffer;
    context.commandEncoder = _encoder;
    return true;
}

void nsMetalRenderDevice::EnsureDepthStencilTexture(int width, int height) {
	if (_depthStencilTexture &&
	    _depthStencilTexture.width == width &&
	    _depthStencilTexture.height == height) {
	    return;
	}

	nsMemoryLoopAllocScope metalAllocScope;
	MTLTextureDescriptor *desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float_Stencil8
	                                                                                width:width
	                                                                               height:height
                                                                                mipmapped:NO];
    desc.usage = MTLTextureUsageRenderTarget;
    _depthStencilTexture = [_device newTextureWithDescriptor:desc];
}

MTLRenderPassDescriptor *nsMetalRenderDevice::CreatePassDescriptor(id<MTLTexture> colorTexture,
                                                                    id<MTLTexture> depthStencilTexture) {
    MTLRenderPassDescriptor *passDesc = _passDescriptor ?: [MTLRenderPassDescriptor renderPassDescriptor];
    passDesc.colorAttachments[0].texture = colorTexture;
    passDesc.colorAttachments[0].loadAction = (_pendingClearFlags & CLR_CBUFF)
        ? MTLLoadActionClear
        : MTLLoadActionLoad;
    passDesc.colorAttachments[0].storeAction = MTLStoreActionStore;
    passDesc.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 0);

    if (depthStencilTexture) {
        passDesc.depthAttachment.texture = depthStencilTexture;
        passDesc.depthAttachment.loadAction = (_pendingClearFlags & CLR_ZBUFF)
            ? MTLLoadActionClear
            : MTLLoadActionLoad;
        passDesc.depthAttachment.storeAction = MTLStoreActionStore;
        passDesc.depthAttachment.clearDepth = 1.0;

        passDesc.stencilAttachment.texture = depthStencilTexture;
        passDesc.stencilAttachment.loadAction = (_pendingClearFlags & CLR_STENCIL)
            ? MTLLoadActionClear
            : MTLLoadActionLoad;
        passDesc.stencilAttachment.storeAction = MTLStoreActionStore;
        passDesc.stencilAttachment.clearStencil = 0;
    } else {
        passDesc.depthAttachment.texture = nil;
        passDesc.stencilAttachment.texture = nil;
    }

    _pendingClearFlags = 0;
    return passDesc;
}

void nsMetalRenderDevice::DrawQuad(vbVertex_t v[4]) {
    auto drawVerts = _quadBuff->GetWriteVertices();
    for (int i = 0; i < 4; ++i) {
        drawVerts[i] = v[i];
    }
    VerticesDraw(_quadBuff);
}
