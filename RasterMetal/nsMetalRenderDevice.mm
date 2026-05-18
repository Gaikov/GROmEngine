//
// Created by OpenCode
//

#include "nsMetalRenderDevice.h"
#include "nsMetalRenderState.h"
#include "nsMetalStencilState.h"
#include "nsLib/log.h"
#include "Engine/Platform.h"
#import <AppKit/AppKit.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
extern "C" id glfwGetCocoaWindow(GLFWwindow* window);

@interface nsMetalView : MTKView
@end

@implementation nsMetalView
- (void)drawRect:(NSRect)dirtyRect {
    // Never draw automatically — fully manual rendering
}
@end

nsMetalRenderDevice::nsMetalRenderDevice()
    : _projMatrix(1), _viewMatrix(1) {}

nsMetalRenderDevice::~nsMetalRenderDevice() {
    Release();
}

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

    _commandQueue = [_device newCommandQueue];

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
            _mtkView.enableSetNeedsDisplay = NO;
            _mtkView.paused = NO;
            _mtkView.autoResizeDrawable = NO;
            _mtkView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
            [view addSubview:_mtkView positioned:NSWindowAbove relativeTo:nil];
        }
    }

    _textures = new nsMetalTexturesCache(_device);
    _programs = new nsMetalProgramsCache(_device);
    if (!_programs->Init()) {
        return false;
    }

    _defaultState = new nsMetalRenderState(_device, *_programs);
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

    _currentState = nullptr;
    for (auto &it : _stateCache) delete it.second;
    _stateCache.clear();
    delete _defaultState; _defaultState = nullptr;

    delete _programs; _programs = nullptr;
    delete _textures; _textures = nullptr;
    delete _quadBuff; _quadBuff = nullptr;

    for (auto vb : _allocatedVBS) delete vb;
    _allocatedVBS.clear();

    _commandQueue = nil;
    _mtkView = nil;
    _device = nil;
}

void nsMetalRenderDevice::InvalidateResources() {}
void nsMetalRenderDevice::GetDisplayInfo(DisplayInfo &info) {}
const rasterConfig_t* nsMetalRenderDevice::GetCurrentConfig() {
    static rasterConfig_t cfg = {32, 800, 600};
    return &cfg;
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

    _commandBuffer = [_commandQueue commandBuffer];
    _currentDrawable = _mtkView.currentDrawable;
    if (!_currentDrawable || _currentDrawable.texture.width == 0 || _currentDrawable.texture.height == 0) {
        _currentDrawable = nil;
        _commandBuffer = nil;
        return false;
    }

    MTLRenderPassDescriptor *passDesc = [MTLRenderPassDescriptor renderPassDescriptor];
    passDesc.colorAttachments[0].texture = _currentDrawable.texture;
    passDesc.colorAttachments[0].loadAction = MTLLoadActionClear;
    passDesc.colorAttachments[0].storeAction = MTLStoreActionStore;
    passDesc.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);

    _encoder = [_commandBuffer renderCommandEncoderWithDescriptor:passDesc];
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

bool nsMetalRenderDevice::BeginScene() {
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

void nsMetalRenderDevice::ClearScene(uint flags) {}

ITexture* nsMetalRenderDevice::TextureLoad(const char *filename, bool, texfmt_t, int flags) {
    return _textures->GetTexture(filename, flags);
}

const char* nsMetalRenderDevice::TextureGetPath(ITexture *t) { return nullptr; }
ITexture* nsMetalRenderDevice::TextureGenerate(int, int, const void *, texfmt_t, bool) { return nullptr; }
void nsMetalRenderDevice::TextureRelease(ITexture *) {}

void nsMetalRenderDevice::TextureBind(ITexture *texture) {
    if (!_encoder) return;
    auto tex = dynamic_cast<nsMetalTexture*>(texture);
    _textures->BindTexture(tex, _encoder);
    _programs->SetTextureBound(tex != nullptr);
}

void nsMetalRenderDevice::TextureTranform(const float *offs2, const float *scale2) {
    nsMatrix m;
    m.Identity();
    if (scale2) { m.Scale(scale2[0], scale2[1], 1.0f); }
    if (offs2) { m._41 = offs2[0]; m._42 = offs2[1]; }
    _programs->SetTextureMatrix(m);
}

IRenState* nsMetalRenderDevice::StateLoad(const char *fileName) {
    if (!fileName || !*fileName) return nullptr;
    auto it = _stateCache.find(fileName);
    if (it != _stateCache.end()) return it->second;
    auto state = new nsMetalRenderState(_device, *_programs);
    if (!state->Load(fileName)) {
        delete state;
        return nullptr;
    }
    _stateCache[fileName] = state;
    return state;
}

const char* nsMetalRenderDevice::StateGetPath(IRenState *s) {
    auto state = dynamic_cast<nsMetalRenderState*>(s);
    return state ? state->GetPath() : "";
}

void nsMetalRenderDevice::StateRelease(IRenState *) {}

void nsMetalRenderDevice::StateApply(IRenState *s) {
    auto state = dynamic_cast<nsMetalRenderState*>(s);
    if (!state) state = _defaultState;
    if (state == _currentState) return;

    if (_encoder) {
        state->Apply(_encoder, *_programs, _currentState);
    }
    _currentState = state;

    if (state->IsAlphaTest()) {
        _programs->SetAlphaCutoff(state->GetAlphaCutoff());
    } else {
        _programs->SetAlphaCutoff(0);
    }
}

void nsMetalRenderDevice::LoadProjMatrix(const float *m) { _projMatrix = m; }
void nsMetalRenderDevice::LoadViewMartix(const float *m) {
    _viewMatrix = m;
    nsMatrix projView = _viewMatrix * _projMatrix;
    _programs->SetProjViewMatrix(projView);
}
void nsMetalRenderDevice::LoadMatrix(const float *m) { _programs->SetModelMatrix(m); }

IVertexBuffer* nsMetalRenderDevice::VerticesCreate(uint vertsCount, uint indexCount, bool, bool useColors) {
    auto vb = new nsMetalVertexBuffer(_device, _textures, vertsCount, indexCount, useColors);
    _allocatedVBS.push_back(vb);
    return vb;
}

void nsMetalRenderDevice::VerticesRelease(IVertexBuffer *vb) {
    auto it = std::find(_allocatedVBS.begin(), _allocatedVBS.end(), vb);
    if (it != _allocatedVBS.end()) _allocatedVBS.erase(it);
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
    auto tex = _textures->GetBoundTexture();
    if (tex) tex->Bind(_encoder, 0);
    program->SetHasTexture(_textures->HasBoundTexture());
    program->UploadUniforms(_encoder);
    auto metalVB = dynamic_cast<nsMetalVertexBuffer*>(vb);
    if (metalVB) {
        metalVB->UseColor(_currentColor);
        metalVB->Draw(_encoder);
    }
}

IStencilState* nsMetalRenderDevice::StencilLoad(const char *) { return new nsMetalStencilState(); }
void nsMetalRenderDevice::StencilRelease(IStencilState *) {}
void nsMetalRenderDevice::StencilApply(IStencilState *) {}

IRenderTexture* nsMetalRenderDevice::RenderTextureCreate(int, int, texfmt_t) { return nullptr; }
void nsMetalRenderDevice::RenderTextureBind(IRenderTexture *) {}
