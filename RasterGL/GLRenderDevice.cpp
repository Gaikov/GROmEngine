//
// Created by Roman on 10/07/2019.
//

#include "GLRenderDevice.h"
#include "GLTexture.h"
#include "GLVertexBuffer.h"
#include "GLUtils.h"
#include "Core/Config.h"

static GLRenderDevice *g_shared = nullptr;

IRenDevice *GetRenDevice()
{
	if (!g_shared)
	{
        g_shared = new GLRenderDevice();
	}
	return g_shared;
}

GLRenderDevice::GLRenderDevice() :
		_quadBuff(nullptr),
		_projMatrix(1),
		_viewMatrix(1)
{
}

bool GLRenderDevice::Init(void *wnd)
{
	LogPrintf(PRN_ALL, "*****************************************\n");
	LogPrintf(PRN_ALL, "+       initializing GL renderer        +\n");
	LogPrintf(PRN_ALL, "*****************************************\n");
	g_cfg->RegCmd("r_restart", [this](int argc, const char* argv[]) {_queryRestart = true;});

	if (!_modes.Init())
	{
		return false;
	}

	LogPrintf(PRN_ALL, "GL Vendor: %s\n", glGetString(GL_VENDOR));
	LogPrintf(PRN_ALL, "GL Renderer: %s\n", glGetString(GL_RENDERER));
	LogPrintf(PRN_ALL, "GL Version: %s\n", glGetString(GL_VERSION));

	_debug.Init(&_textures);

	_quadBuff = new GLVertexBuffer(&_textures, 4, 6, false);

	// 3 |-------| 2
	//   |      /|
	//   |    /  |
	//   |  /    |
	//   |/      |
	// 0 |-------| 1

	auto i = _quadBuff->GetWriteIndices();
	i[0] = 0;
	i[1] = 1;
	i[2] = 2;

	i[3] = 0;
	i[4] = 2;
	i[5] = 3;

	_shaders.Init();
    _stencils.Init();

	return PrepareOpenGL();
}

bool GLRenderDevice::PrepareOpenGL()
{
	if (!_modes.ApplyCurrentMode())
	{
		return false;
	}

	glDepthFunc(GL_LEQUAL);
    glClearColor(0, 0, 0, 1);

	return true;
}

void GLRenderDevice::CleanupOpenGL()
{
	//TODO: destroy gl context
    _shaders.Apply(nullptr);
    _textures.BindTexture(nullptr);
}

void GLRenderDevice::Release()
{
	LogPrintf(PRN_ALL, "*****************************************\n");
	LogPrintf(PRN_ALL, "+         releasing GL renderer         +\n");
	LogPrintf(PRN_ALL, "*****************************************\n");

	CleanupOpenGL();

	delete _quadBuff;

	LogPrintf(PRN_ALL, "...releasing shaders\n");
	_shaders.ReleaseAll();

	LogPrintf(PRN_ALL, "...releasing textures\n");
	_textures.ReleaseAll();

	_debug.Release();

	delete g_shared;
    g_shared = nullptr;
}

void GLRenderDevice::GetDisplayInfo(DisplayInfo &info)
{
	auto &modes = _modes.GetModes();
    for (auto &mode : modes) {
        DisplayMode m = {
                mode.width,
                mode.height
        };
        info.modes.push_back(m);
    }
}

const rasterConfig_t *GLRenderDevice::GetCurrentConfig()
{
	return &_modes.GetConfig();
}

void GLRenderDevice::SetColor(const float *c)
{
	glColor4f(c[0], c[1], c[2], c[3]);
}

ITexture *GLRenderDevice::TextureLoad(const char *filename, bool mipmap, texfmt_t fmt)
{
	return _textures.GetResource(filename, 0);
}

ITexture *GLRenderDevice::TextureGenerate(int width, int height, const void *data, texfmt_t fmt, bool mipmap)
{
	auto bmData = nsBitmapData::Create(width, height);
	bmData->SetData(data);
	return _textures.CreateFromBitmapData(bmData);
}

void GLRenderDevice::TextureRelease(ITexture *texture)
{
	auto t = dynamic_cast<GLTexture *>(texture);
	if (t)
	{
		_textures.ReleaseResource(t);
	}
}

void GLRenderDevice::TextureBind(ITexture *texture)
{
	auto t = dynamic_cast<GLTexture *>(texture);
	if (_textures.BindTexture(t))
	{
		_shaders.ApplyTextureParams();
	}
}

void GLRenderDevice::TextureTranform(const float *offs2, const float *scale2)
{
    glMatrixMode(GL_TEXTURE);
    nsMatrix m;
    m.Identity();
    if (offs2) m.SetPos(offs2);
    if (scale2) m.Scale(scale2[0], scale2[1], 1);
    glLoadMatrixf(m);
    glMatrixMode(GL_MODELVIEW);
}

IRenState *GLRenderDevice::StateLoad(const char *fileName)
{
	return _shaders.GetResource(fileName, 0);
}

void GLRenderDevice::StateRelease(IRenState *state)
{
	auto shader = dynamic_cast<GLShader *>(state);
	_shaders.ReleaseResource(shader);
}

void GLRenderDevice::StateApply(IRenState *state)
{
	auto shader = dynamic_cast<GLShader *>(state);
	_shaders.Apply(shader);
}

void GLRenderDevice::ClearScene(uint flags)
{
	GLbitfield glBits    = 0;

	if (CLR_CBUFF & flags)
	{
		glBits |= GL_COLOR_BUFFER_BIT;
	}
	if (CLR_ZBUFF & flags)
	{
		glBits |= GL_DEPTH_BUFFER_BIT;
		glDepthMask(GL_TRUE);
	}
    if (CLR_STENCIL & flags) {
        glBits |= GL_STENCIL_BUFFER_BIT;
    }
	glClear(glBits);
	GL_CHECK_R("glClear",)
}

bool GLRenderDevice::BeginScene()
{
	if (_queryRestart)
	{
		_queryRestart = false;
		CleanupOpenGL();
		if (!PrepareOpenGL())
		{
			return false;
		}
	}

	int w, h;
	App_GetPlatform()->GetClientSize(w, h);
	glViewport(0, 0, w, h);

	return true;
}

void GLRenderDevice::EndScene()
{
	glFlush();
}

void GLRenderDevice::ApplyProjectionMatrix()
{
	nsMatrix projView = _viewMatrix * _projMatrix;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(projView);
	glMatrixMode(GL_MODELVIEW);
}

void GLRenderDevice::LoadProjMatrix(const float *m)
{
	_projMatrix = m;
	ApplyProjectionMatrix();
}

void GLRenderDevice::LoadViewMartix(const float *m)
{
	_viewMatrix = m;
	ApplyProjectionMatrix();
}

void GLRenderDevice::LoadMatrix(const float *m)
{
	glLoadMatrixf(m);
}

void GLRenderDevice::MultMatrixLocal(const float *m)
{
	glMultMatrixf(m);
}

void GLRenderDevice::DrawLinedSprite(float x1, float y1, float x2, float y2, float width, float height)
{
	nsVec2 p1(x1, y1);
	nsVec2 dir = (nsVec2(x2, y2) - p1) * 0.5f;
	p1 += dir;
	dir.Norm();

	nsVec2 down = dir.GetRight();

	p1 -= dir * width * 0.5f;
	p1 -= down * height * 0.5f;

	dir         = dir * width;
	down        = down * height;

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

void GLRenderDevice::DrawCharScaled(float x, float y, rchar_t *ch, float sx, float sy)
{
	if (!ch)
	{
		return;
	}

	float      size_x       = ch->size[0] * sx;
	float      size_y       = ch->size[1] * sy;
	float      posX         = x + ch->offs[0] * sx;
	float      posY         = y + ch->offs[1] * sy;
	nsVec3     zero;
	dword      c            = 0xffffffff;
	vbVertex_t charVerts[4] =
			           {
					           {nsVec3(posX, posY, 0),                   zero, c, ch->coord[0],
							                                                                       ch->coord[1] +
							                                                                       ch->tex_size[1]},
					           {nsVec3(size_x + posX, posY, 0),          zero, c, ch->coord[0] +
					                                                              ch->tex_size[0], ch->coord[1] +
					                                                                               ch->tex_size[1]},
					           {nsVec3(size_x + posX, size_y + posY, 0), zero, c, ch->coord[0] +
					                                                              ch->tex_size[0], ch->coord[1]},
					           {nsVec3(posX, size_y + posY,
					                   0),                               zero, c, ch->coord[0],    ch->coord[1]}//*/
			           };

	DrawQuad(charVerts);
}

IVertexBuffer *GLRenderDevice::VerticesCreate(uint vertsCount, uint indexCount, bool dynamic, bool useColors)
{
	return new GLVertexBuffer(&_textures, vertsCount, indexCount, useColors);
}

void GLRenderDevice::VerticesRelease(IVertexBuffer *vb)
{
	auto glvb = dynamic_cast<GLVertexBuffer *>(vb);
	delete glvb;
}

void GLRenderDevice::VerticesDraw(IVertexBuffer *vb)
{
	auto glvb = dynamic_cast<GLVertexBuffer *>(vb);
	glvb->Draw();
}

void GLRenderDevice::DrawSprite3D(const nsVec3 &pos, float width, float height, float angle)
{
	auto   x        = _viewMatrix.GetViewX();
	auto   y        = _viewMatrix.GetViewY();

	if (angle != 0.0f)
	{
		nsMatrix m;
		m.AxisRotate(y.Cross(x), angle);
		y *= m;
		x *= m;
	}

	nsVec3 down     = -y * width;
	nsVec3 right    = x * height;
	nsVec3 origin   = pos - right / 2.0f;
	origin -= down / 2.0f;

	nsVec3     zero;
	auto       c    = 0xffffffff;
	vbVertex_t v[4] =
			           {
					           {origin,                zero, c, 0, 0},
					           {origin + down,         zero, c, 0, 1},
					           {origin + down + right, zero, c, 1, 1},
					           {origin + right,        zero, c, 1, 0}
			           };

	LoadMatrix(nsMatrix::identity);
	DrawQuad(v);
}

void GLRenderDevice::DrawChar3D(const rchar_t *ch, const nsVec3 &pos, float sx, float sy)
{
	if (!ch) return;

	float      w = ch->size[0] * sx;
	float      h = ch->size[1] * sy;
	vbVertex_t v[4];
	v[0].tu  = ch->coord[0];
	v[0].tv  = ch->coord[1] + ch->tex_size[1];

	v[1].tu  = ch->coord[0] + ch->tex_size[0];
	v[1].tv  = ch->coord[1] + ch->tex_size[1];

	v[2].tu  = ch->coord[0] + ch->tex_size[0];
	v[2].tv  = ch->coord[1];

	v[3].tu  = ch->coord[0];
	v[3].tv  = ch->coord[1];

	v[0].v.x = pos.x;
	v[0].v.y = pos.y;
	v[0].v.z = pos.z;

	v[1].v.x = pos.x + w;
	v[1].v.y = pos.y;
	v[1].v.z = pos.z;

	v[2].v.x = pos.x + w;
	v[2].v.y = pos.y + h;
	v[2].v.z = pos.z;

	v[3].v.x = pos.x;
	v[3].v.y = pos.y + h;
	v[3].v.z = pos.z;

	DrawQuad(v);
}

void
GLRenderDevice::DrawPlaneSprite(const nsVec3 &p1, const nsVec3 &p2, float width, uint wrap_count)
{
	nsVec3 l, w, dir;

	dir = -_viewMatrix.GetViewZ();

	l   = p2 - p1;

	w   = dir.Cross(l);
	w.Norm();
	w *= width / 2.0f;

	vbVertex_t v[4];
	v[0].tu = 0;
	v[0].tv = 0;
	v[1].tu = 0;
	v[1].tv = (float) wrap_count;
	v[2].tu = 1;
	v[2].tv = (float) wrap_count;
	v[3].tu = 1;
	v[3].tv = 0;

	v[0].c  = v[1].c = v[2].c = v[3].c = 0xffffffff;

	v[0].v  = p2 - w;
	v[1].v  = p1 - w;
	v[2].v  = p1 + w;
	v[3].v  = p2 + w;

	LoadMatrix(nsMatrix::identity);
	DrawQuad(v);
}

void GLRenderDevice::Lighting(bool enable)
{
	_light.Lighting(enable);
}

void GLRenderDevice::LightEnable(int source, const nsVec3 &dir, const float *c)
{
	_light.LightEnable(source, dir, c);
}

void GLRenderDevice::LightDisable(int source)
{
	_light.LightDisable(source);
}

void GLRenderDevice::SetLightAmbient(const float *c)
{
	_light.SetLightAmbient(c);
}

void GLRenderDevice::DrawQuad(vbVertex_t v[4])
{
	vbVertex_t *drawVerts = _quadBuff->GetWriteVertices();
	for (int   i          = 0; i < 4; i++)
	{
		drawVerts[i] = v[i];
	}

	_quadBuff->Draw();
}

void GLRenderDevice::InvalidateResources() {
    _queryRestart = true;
    _textures.UnloadFromGPU();
}

IStencilState *GLRenderDevice::StencilLoad(const char *fileName) {
    return _stencils.GetResource(fileName, 0);
}

void GLRenderDevice::StencilRelease(IStencilState *state) {
    _stencils.ReleaseResource(dynamic_cast<nsGLStencilState *>(state), false);
}

void GLRenderDevice::StencilApply(IStencilState *state) {
    _stencils.Apply(dynamic_cast<nsGLStencilState*>(state));
}



