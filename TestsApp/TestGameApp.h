//
// Created by Roman on 09/07/2019.
//

#ifndef TEST_TESTGAMEAPP_H
#define TEST_TESTGAMEAPP_H

#include "TestCommon.h"

class TestGameApp : public BaseTestApp
{
protected:
	void RegisterTests() override;

	void Release() override;

};


#endif //TEST_TESTGAMEAPP_H
