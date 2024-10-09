//
// Created by Roman on 08/09/2019.
//

#ifndef __PLANESPRITE3DTEST_H__
#define __PLANESPRITE3DTEST_H__

#include "TestCommon.h"

class PlaneSprite3dTest : public BaseFunctional3dTest
{
public:
	PlaneSprite3dTest();
	bool Init() override;
	void Draw() override;
private:
	ITexture    *_tex;

};

#endif //__PLANESPRITE3DTEST_H__
