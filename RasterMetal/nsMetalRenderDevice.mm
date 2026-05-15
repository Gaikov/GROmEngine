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
            _metalLayer = [CAMetalLayer layer];
            _metalLayer.device = _device;
            _metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
            _metalLayer.framebufferOnly = NO;
            _metalLayer.contentsScale = [window backingScaleFactor];

            NSView *view = [window contentView];
            [view setWantsLayer:YES];
            _metalLayer.frame = view.bounds;
            [view.layer addSublayer:_metalLayer];

            _metalLayer.drawableSize = CGSizeMake(
                _metalLayer.frame.size.width * _metalLayer.contentsScale,
                _metalLayer.frame.size.height * _metalLayer.contentsScale);
        }
    }

    _textures = new nsMetalTexturesCache(_device);
    _programs = new nsMetalProgramsCache(_device);
    if (!_programs->Init()) {
        return false;
    }

    _quadBuff = new nsMetalVertexBuffer(_device, _textures, 4, 6, false);
    auto i = _quadBuff->GetWriteIndices();
    i[0] = 0; i[1] = 1; i[2] = 2;
    i[3] = 0; i[4] = 2; i[5] = 3;

    return true;
}

void nsMetalRenderDevice::Release() {
    EndEncoder();
    _commandBuffer = nil;
    _metalLayer = nullptr;

    for (auto vb : _allocatedVBS) delete vb;
    _allocatedVBS.clear();
    delete _quadBuff; _quadBuff = nullptr;
    delete _textures; _textures = nullptr;
    delete _programs; _programs = nullptr;

    _commandQueue = nil;
    _device = nil;
}

void nsMetalRenderDevice::InvalidateResources() {}

void nsMetalRenderDevice::GetDisplayInfo(DisplayInfo &info) {}

const rasterConfig_t* nsMetalRenderDevice::GetCurrentConfig() {
    static rasterConfig_t cfg = {32, 800, 600};
    return &cfg;
}

void nsMetalRenderDevice::SetColor(const float c[4]) {}

bool nsMetalRenderDevice::BeginEncoder() {
    _commandBuffer = [_commandQueue commandBuffer];
    _currentDrawable = _metalLayer.nextDrawable;
    if (!_currentDrawable) {
        Log::Warning("Metal: nextDrawable returned nil");
        return false;
    }

    MTLRenderPassDescriptor *passDesc = [MTLRenderPassDescriptor renderPassDescriptor];
    passDesc.colorAttachments[0].texture = _currentDrawable.texture;
    passDesc.colorAttachments[0].loadAction = MTLLoadActionClear;
    passDesc.colorAttachments[0].storeAction = MTLStoreActionStore;
    passDesc.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);

    _encoder = [_commandBuffer renderCommandEncoderWithDescriptor:passDesc];
    return _encoder != nil;
}

void nsMetalRenderDevice::EndEncoder() {
    if (_encoder) {
        [_encoder endEncoding];
        _encoder = nil;
    }
}

bool nsMetalRenderDevice::BeginScene() {
    if (_metalLayer) {
        int w, h;
        App_GetPlatform()->GetClientSize(w, h);
        _metalLayer.drawableSize = CGSizeMake(w, h);
    }
    return BeginEncoder();
}

void nsMetalRenderDevice::EndScene() {
    EndEncoder();
    if (_commandBuffer && _currentDrawable) {
        [_commandBuffer presentDrawable:_currentDrawable];
        [_commandBuffer commit];
        [_commandBuffer waitUntilCompleted];
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

IRenState* nsMetalRenderDevice::StateLoad(const char *) {
    if (auto prog = _programs->GetDefaultProgram()) {
        return new nsMetalRenderState(prog->GetPipelineState());
    }
    return nullptr;
}
const char* nsMetalRenderDevice::StateGetPath(IRenState *) { return ""; }
void nsMetalRenderDevice::StateRelease(IRenState *) {}
void nsMetalRenderDevice::StateApply(IRenState *) {}

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
    auto program = _programs->GetDefaultProgram();
    if (!program) return;
    program->Bind(_encoder);

    auto tex = _textures->GetBoundTexture();
    if (tex) tex->Bind(_encoder, 0);

    program->SetHasTexture(_textures->HasBoundTexture());
    program->UploadUniforms(_encoder);

    auto metalVB = dynamic_cast<nsMetalVertexBuffer*>(vb);
    if (metalVB) metalVB->Draw(_encoder);
}

IStencilState* nsMetalRenderDevice::StencilLoad(const char *) { return new nsMetalStencilState(); }
void nsMetalRenderDevice::StencilRelease(IStencilState *) {}
void nsMetalRenderDevice::StencilApply(IStencilState *) {}

IRenderTexture* nsMetalRenderDevice::RenderTextureCreate(int, int, texfmt_t) { return nullptr; }
void nsMetalRenderDevice::RenderTextureBind(IRenderTexture *) {}
