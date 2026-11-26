// Copyright (c) 2003-2019, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLRenderDevice.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "GLCommon.h"
#include "GLTexturesCache.h"
#include "shaders/GLShadersManager.h"
#include "RasterCommon/DisplayModeConfig.h"
#include "renderTexture/GLRenderTexturesCache.h"
#include "stencil/GLStencilManager.h"

#include <cstdint>

class GLVertexBuffer;

class GLRenderDevice : public IRenDevice
{
public:
	GLRenderDevice();
	bool Init(void *wnd) override;
	void Release() override;
	const rasterConfig_t *GetCurrentConfig() override;
	void SetColor(const float *c) override;
	ITexture *TextureLoad(const char *filename, texfmt_t fmt = TF_RGBA, int flags = TLF_NONE) override;
	const char * TextureGetPath(ITexture *t) override;
	ITexture *TextureGenerate(int width, int height, const void *data, texfmt_t fmt, bool mipmap) override;
	void TextureRelease(ITexture *texture) override;
	void TextureBind(ITexture *texture, unsigned slot = 0) override;
	void TextureTranform(const float *offs2, const float *scale2) override;
	IRenState *StateLoad(const char *fileName) override;
	const char* StateGetPath(IRenState *state) override;
	void StateRelease(IRenState *state) override;
	void StateApply(IRenState *state) override;
    void SetScissorRect(const nsRect &rect) override;
    void ResetScissorRect() override;
    void ClearScene(uint flags, const nsColor &color = {0, 0, 0, 0}) override;
	bool BeginScene() override;
	nsBitmapData::tSP CaptureScreen() override;
	void EndScene() override;
	void LoadProjMatrix(const float *m) override;
	void LoadViewMartix(const float *m) override;
	void LoadMatrix(const float *m) override;
	void DrawLinedSprite(float x1, float y1, float x2, float y2, float width, float height) override;

	IVertexBuffer *VerticesCreate(uint vertsCount, uint indexCount, bool dynamic, bool useColors) override;
	void VerticesRelease(IVertexBuffer *vb) override;
	void VerticesDraw(IVertexBuffer *vb) override;
	void DrawSprite3D(const nsVec3 &pos, float width, float height, float angle) override;
	void DrawChar3D(const rchar_t *ch, const nsVec3 &pos, float sx, float sy) override;
	void DrawPlaneSprite(const nsVec3 &p1, const nsVec3 &p2, float width, uint wrap_count) override;

    IStencilState *StencilLoad(const char *fileName) override;
    void StencilRelease(IStencilState *state) override;
    void StencilApply(IStencilState *state) override;

    void InvalidateResources() override;

	IRenderTexture * RenderTextureCreate(int width, int height, texfmt_t) override;
	void RenderTextureBind(IRenderTexture *rt) override;
	IRenderTexture *GetRenderTarget() const override { return _renderTextures.GetCurrent(); }
    void RenderTextureRelease(IRenderTexture *target) override;

private:

    static constexpr auto SamplersPerState = 2u;
    static constexpr auto SamplerCount = GLShader::SamplerStateCount * SamplersPerState;

	nsDisplayModeConfig _modes;
	GLTexturesCache  _textures;
    GLint            _textureFormat = GL_RGBA8;
	nsGLRenderTexturesCache _renderTextures;
	GLVertexBuffer   *_quadBuff;
	GLShadersManager _shaders;
    nsGLStencilManager _stencils;
	nsMatrix         _projMatrix;
	nsMatrix         _viewMatrix;
	bool             _renderTextureBound = false;
    bool _scissorEnabled = false;
    nsRect _scissorRect;
	bool             _sceneActive = false;
	bool             _queryRestart = false;

	nsColor          _currentColor;
	std::vector<GLVertexBuffer*> _allocatedVBS;
	GLuint           _samplers[SamplerCount] = {};
	GLuint           _boundSamplers[2] = {};
    bool _trilinearFilter = true;
	std::uint64_t    _frameSerial = 0;

private:
	void ApplyProjectionMatrix();
    static nsRect ToGLScissorRect(const nsRect &rect, int targetHeight, bool renderTexture);
    void ApplyScissor();
	void DrawQuad(vbVertex_t v[4]);
	bool PrepareOpenGL();
	void CleanupOpenGL();
	void InitSamplers();
    void UpdateTextureFilter();
	void ReleaseSamplers();
	void ApplySampler();
    static unsigned GetSamplerArrayIndex(unsigned state, bool mipmapped) {
        return state * SamplersPerState + unsigned(mipmapped);
    }
};
