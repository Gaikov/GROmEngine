//
// Created by Roman on 07/09/2019.
//

#ifndef __SPRITE3DTEST_H__
#define __SPRITE3DTEST_H__

#include "TestCommon.h"

class Sprite3dTest : public BaseFunctional3dTest
{
public:
	Sprite3dTest();
	bool Init() override;
	void Loop(float deltaTime) override;
	void Draw() override;
	void Release() override;
private:
	ITexture    *_tex = nullptr;
    IRenState   *_state = nullptr;
	float       _angle = 0;
};


#endif //__SPRITE3DTEST_H__
