//
// Created by Roman Gaikov on 2019-07-12.
//

#include "GLTexture.h"
#include "GLUtils.h"
#include "GLCommon.h"
#include "Engine/Platform.h"
#include "ImageDecoder/BitmapLoader.h"
#include "Core/Package.h"

GLTexture::GLTexture() :
		_glTexture(0)
{
}

GLTexture::~GLTexture()
{
    UnloadFromGPU();
}

GLTexture *GLTexture::Load(const char *filePath)
{
    if (!StrCheck(filePath)) {
        return nullptr;
    }

	LogPrintf(PRN_ALL, "...creating texture: %s\n", filePath);
	auto t = new GLTexture();
	t->CreateFromFile(filePath);
	return t;
}

void GLTexture::Free(GLTexture *t)
{
	LogPrintf(PRN_ALL, "...releasing texture: %s\n", t->_id.c_str());
	delete t;
}

bool GLTexture::CreateFromFile(const char *filePath)
{
    nsFilePath path(filePath);

    auto ext = path.GetExtension();
    ext.ToLower();
    if (ext == "tga") {
        ext = "png";
    } else if (ext == "bmp") {
        ext = "jpg";
    }
    nsString newPath = path.GetPathWithoutExt() + "." + ext.AsChar();
    filePath = newPath;

    _id = filePath;
	return true;
}

bool GLTexture::UploadToGPU() {
    if (!EnsureBitmapData()) {
        return false;
    }

    if (_glTexture || !_bmData.get()) {
        return false;
    }

    //Log::Info("...uploading to GPU: %s", _id.c_str());

    glGenTextures(1, &_glTexture);
    GL_CHECK("glGenTextures")

    glBindTexture(GL_TEXTURE_2D, _glTexture);
    GL_CHECK("glBindTexture")

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 _bmData->GetWidth(), _bmData->GetHeight(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, _bmData->GetData());
    GL_CHECK("glTexImage2D")

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL_CHECK("glTexParameteri")

    return true;
}


bool GLTexture::Bind()
{
    UploadToGPU();

	glBindTexture(GL_TEXTURE_2D, _glTexture);
	GL_CHECK("glBindTexture")

	return true;
}

int GLTexture::GetWidth()
{
    EnsureBitmapData();
	return _bmData ? _bmData->GetWidth() : 1;
}

int GLTexture::GetHeight()
{
    EnsureBitmapData();
	return _bmData ? _bmData->GetHeight() : 1;
}

void GLTexture::GetSize(int &width, int &height)
{
    EnsureBitmapData();

    if (_bmData) {
        width  = _bmData->GetWidth();
        height = _bmData->GetHeight();
    } else {
        width = 1;
        height = 1;
    }
}

GLTexture *GLTexture::Create(const char *id, nsBitmapData::tSP& data)
{
	auto t = new GLTexture();
	t->_id = id;
    assert(data.get());
    t->_bmData = data;
	return t;
}

void GLTexture::UnloadFromGPU() {
    if (_glTexture) {
        glDeleteTextures(1, &_glTexture);
        _glTexture = 0;
    }
}

bool GLTexture::EnsureBitmapData() {
    if (_bmData.get()) {
        return true;
    }

    Log::Debug("...loading bitmap data: %s", _id.c_str());
    _bmData = BitmapLoader::LoadFromFile(_id.c_str());
    return _bmData.get();
}

