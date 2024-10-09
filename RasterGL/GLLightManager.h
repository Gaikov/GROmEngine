//
// Created by Roman Gaikov on 2019-09-12.
//

#ifndef __GLLIGHTMANAGER_H__
#define __GLLIGHTMANAGER_H__

#include "GLCommon.h"

class GLLightManager
{
public:
	GLLightManager() = default;
	void Lighting(bool enable);
	void LightEnable(int source, const nsVec3 &dir, const float *c);
	void LightDisable(int source);
	void SetLightAmbient(const float *c);

private:
	struct nsLight
	{
		nsVec3  dir;
		nsColor color;
	};
	enum
	{
		MAX_LIGHTS = 8
	};
	nsLight _lights[MAX_LIGHTS];

private:
	void ApplyLight(int source);
};


#endif //__GLLIGHTMANAGER_H__
