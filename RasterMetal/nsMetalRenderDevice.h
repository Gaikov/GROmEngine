//
// Created by OpenCode
//
#pragma once

#include "Engine/RenDevice.h"
#include "nsMetalVertexBuffer.h"
#include "nsMetalTexturesCache.h"
#include "nsMetalProgramsCache.h"
#include "nsMetalRenderState.h"
#include "nsMetalStencilState.h"
#include "nsMetalRenderTexture.h"
#include "nsMetalDisplayModes.h"
#include "nsLib/color.h"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

struct nsMetalFrameContext {
    __unsafe_unretained id<MTLDevice> device = nil;
    __unsafe_unretained MTLRenderPassDescriptor *renderPassDescriptor = nil;
    __unsafe_unretained id<MTLCommandBuffer> commandBuffer = nil;
    __unsafe_unretained id<MTLRenderCommandEncoder> commandEncoder = nil;
};

class nsMetalRenderDevice : public IRenDevice {
public:
    nsMetalRenderDevice();
    ~nsMetalRenderDevice() override;

    bool Init(void *wnd) override;
    void Release() override;
    void InvalidateResources() override;

    void GetDisplayInfo(DisplayInfo &info) override;
    const rasterConfig_t* GetCurrentConfig() override;

    void SetColor(const float c[4]) override;

    ITexture* TextureLoad(const char *filename, bool mipmap, texfmt_t fmt, int flags) override;
    const char* TextureGetPath(ITexture *t) override;
    ITexture* TextureGenerate(int width, int height, const void *data, texfmt_t fmt, bool mipmap) override;
    void TextureRelease(ITexture *texture) override;
    void TextureBind(ITexture *texture) override;
    void TextureTranform(const float *offs2, const float *scale2) override;

    IRenState* StateLoad(const char *fileName) override;
    const char* StateGetPath(IRenState *state) override;
    void StateRelease(IRenState *state) override;
    void StateApply(IRenState *state) override;

    void ClearScene(uint flags) override;
    bool BeginScene() override;
    void EndScene() override;

    void LoadProjMatrix(const float *m) override;
    void LoadViewMartix(const float *m) override;
    void LoadMatrix(const float *m) override;

    IVertexBuffer* VerticesCreate(uint vertsCount, uint indexCount, bool dynamic, bool useColors) override;
    void VerticesRelease(IVertexBuffer *vb) override;
    void VerticesDraw(IVertexBuffer *vb) override;

    void DrawLinedSprite(float x1, float y1, float x2, float y2, float width, float height) override;
    void DrawSprite3D(const nsVec3 &pos, float width, float height, float angle) override;
    void DrawChar3D(const rchar_t *ch, const nsVec3 &pos, float sx, float sy) override;
    void DrawPlaneSprite(const nsVec3 &p1, const nsVec3 &p2, float width, uint wrap_count) override;

    IStencilState* StencilLoad(const char *fileName) override;
    void StencilRelease(IStencilState *state) override;
    void StencilApply(IStencilState *state) override;

    void Lighting(bool enable) override {}
    void LightEnable(int source, const nsVec3 &dir, const float *c) override {}
    void LightDisable(int source) override {}
    void SetLightAmbient(const float *c) override {}

    IRenderTexture* RenderTextureCreate(int width, int height, texfmt_t fmt) override;
    void RenderTextureBind(IRenderTexture *rt) override;

    id<MTLDevice> GetMetalDevice() const { return _device; }
    bool GetActiveFrameContext(nsMetalFrameContext &context) const;

private:
    id<MTLDevice>               _device          = nil;
    id<MTLCommandQueue>         _commandQueue    = nil;
    MTKView                     *_mtkView         = nil;
    id<MTLCommandBuffer>        _commandBuffer   = nil;
    id<MTLRenderCommandEncoder> _encoder         = nil;
    id<CAMetalDrawable>         _currentDrawable = nil;
    id<MTLTexture>              _depthStencilTexture = nil;
    MTLRenderPassDescriptor    *_passDescriptor  = nil;

    nsMetalTexturesCache       *_textures        = nullptr;
    nsMetalProgramsCache       *_programs        = nullptr;
    nsMetalVertexBuffer        *_quadBuff        = nullptr;

	std::vector<nsMetalVertexBuffer*> _allocatedVBS;
	std::vector<nsMetalRenderTexture*> _allocatedRenderTextures;
	std::map<std::string, nsMetalRenderState*> _stateCache;
    std::map<nsMetalRenderState*, uint> _stateRefs;

    nsMetalRenderState         *_defaultState    = nullptr;
	nsMetalRenderState         *_currentState    = nullptr;
	nsMetalStencilState        *_currentStencil  = nullptr;
	nsMetalRenderTexture       *_currentRenderTexture = nullptr;
	nsMetalDisplayModes        _displayModes;

	nsMatrix _projMatrix;
	nsMatrix _viewMatrix;
	nsColor  _currentColor = nsColor::white;
	uint _pendingClearFlags = CLR_ALL;
	uint _frameIndex = 0;
	bool _queryRestart = false;

	bool BeginEncoder();
	void EndEncoder();
	bool ApplyDisplayMode();
	void ApplyVSync();
	void RestartResources();
	void EnsureDepthStencilTexture(int width, int height);
    MTLRenderPassDescriptor *CreatePassDescriptor(id<MTLTexture> colorTexture,
                                                  id<MTLTexture> depthStencilTexture);
    void ApplyProjectionMatrix();
    void DrawQuad(vbVertex_t v[4]);
};
