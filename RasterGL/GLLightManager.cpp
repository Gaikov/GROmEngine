//
// Created by Roman Gaikov on 2019-09-12.
//

#include "GLLightManager.h"
#include "GLUtils.h"

//https://www.glprogramming.com/red/chapter05.html


void GLLightManager::Lighting(bool enable)
{
	GLUtils::SetState(GL_LIGHTING, enable);
}

void GLLightManager::LightEnable(int source, const nsVec3 &dir, const float *c)
{
	nsLight &l = _lights[source];
	l.dir = -dir;
	l.dir.Norm();
	l.color = c;

	glEnable(GL_LIGHT0 + source);
	GL_CHECK_R("glEnable(GL_LIGHT)",)
	ApplyLight(source);
}

void GLLightManager::LightDisable(int source)
{
	glDisable(GL_LIGHT0 + source);
}

void GLLightManager::SetLightAmbient(const float *c)
{
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, c);
}

void GLLightManager::ApplyLight(int source)
{
	nsLight &l = _lights[source];
	source += GL_LIGHT0;

	GLfloat dir[] = {l.dir.x, l.dir.y, l.dir.z, 0};
	glLightfv(source, GL_POSITION, dir);
	glLightfv(source, GL_DIFFUSE, l.color);

	GL_CHECK_R("ApplyLight",)
}