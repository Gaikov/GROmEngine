//
// Created by Roman on 10/07/2019.
//

#ifndef TEST_GLRENDERDEVICE_H
#define TEST_GLRENDERDEVICE_H

#include "GLCommon.h"
#include "GLTexturesCache.h"
#include "shaders/GLShadersManager.h"
#include "GLLightManager.h"
#include "GLDisplayModes.h"
#include "GLDebug.h"
#include "renderTexture/GLRenderTexturesCache.h"
#include "shaders/GLProgram.h"
#include "stencil/GLStencilManager.h"

class GLVertexBuffer;

class GLRenderDevice : public IRenDevice
{
public:
	GLRenderDevice();
	bool Init(void *wnd) override;
	void Release() override;
	void GetDisplayInfo(DisplayInfo &info) override;
	const rasterConfig_t *GetCurrentConfig() override;
	void SetColor(const float *c) override;
	ITexture *TextureLoad(const char *filename, bool mipmap, texfmt_t fmt) override;
	ITexture *TextureGenerate(int width, int height, const void *data, texfmt_t fmt, bool mipmap) override;
	void TextureRelease(ITexture *texture) override;
	void TextureBind(ITexture *texture) override;
	void TextureTranform(const float *offs2, const float *scale2) override;
	IRenState *StateLoad(const char *fileName) override;
	void StateRelease(IRenState *state) override;
	void StateApply(IRenState *state) override;
	void ClearScene(uint flags) override;
	bool BeginScene() override;
	void EndScene() override;
	void LoadProjMatrix(const float *m) override;
	void LoadViewMartix(const float *m) override;
	void LoadMatrix(const float *m) override;
	void MultMatrixLocal(const float *m) override;
	void DrawLinedSprite(float x1, float y1, float x2, float y2, float width, float height) override;
	void DrawCharScaled(float x, float y, rchar_t *ch, float sx, float sy) override;
	IVertexBuffer *VerticesCreate(uint vertsCount, uint indexCount, bool dynamic, bool useColors) override;
	void VerticesRelease(IVertexBuffer *vb) override;
	void VerticesDraw(IVertexBuffer *vb) override;
	void DrawSprite3D(const nsVec3 &pos, float width, float height, float angle) override;
	void DrawChar3D(const rchar_t *ch, const nsVec3 &pos, float sx, float sy) override;
	void DrawPlaneSprite(const nsVec3 &p1, const nsVec3 &p2, float width, uint wrap_count) override;
	void Lighting(bool enable) override;
	void LightEnable(int source, const nsVec3 &dir, const float *c) override;
	void LightDisable(int source) override;
	void SetLightAmbient(const float *c) override;

    IStencilState *StencilLoad(const char *fileName) override;
    void StencilRelease(IStencilState *state) override;
    void StencilApply(IStencilState *state) override;

    void InvalidateResources() override;

	IRenderTexture * RenderTextureCreate(int width, int height, texfmt_t) override;
	void RenderTextureBind(IRenderTexture *rt) override;

private:

	GLDisplayModes   _modes;
	GLTexturesCache  _textures;
	nsGLRenderTexturesCache _renderTextures;
	GLVertexBuffer   *_quadBuff;
	GLShadersManager _shaders;
    nsGLStencilManager _stencils;
	GLLightManager   _light;
	nsMatrix         _projMatrix;
	nsMatrix         _viewMatrix;
	bool             _queryRestart = false;
    GLDebug          _debug;
	nsGLProgram		 _defaultProgram;
	nsColor          _currentColor;

private:
	void ApplyProjectionMatrix();
	void DrawQuad(vbVertex_t v[4]);
	bool PrepareOpenGL();
	void CleanupOpenGL();

};

#endif //TEST_GLRENDERDEVICE_H
