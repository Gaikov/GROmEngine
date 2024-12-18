//
// Created by Roman Gaikov on 2019-07-12.
//

#ifndef _GROM_GLTEXTURE_H
#define _GROM_GLTEXTURE_H

#include "GLCommon.h"

//https://community.khronos.org/t/loading-texture-using-sdl/68644

class GLTexture : public ITexture
{
public:
	static GLTexture *Load(const char *filePath);
	static void Free(GLTexture *t);
	static GLTexture *Create(const char *id, nsBitmapData::tSP &data);
	bool Bind();
	int GetWidth();
	int GetHeight();
	void GetSize(int &width, int &height) override;

    void UnloadFromGPU();

private:
	GLTexture();
	virtual ~GLTexture();
	bool CreateFromFile(const char *filePath);
    bool UploadToGPU();
    bool EnsureBitmapData();

private:
	std::string        _id;
	GLuint             _glTexture;
	nsBitmapData::tSP _bmData;
};


#endif //_GROM_GLTEXTURE_H
