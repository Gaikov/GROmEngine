//
// Created by Roman on 11/08/2019.
//

#pragma once
#include "Engine/UserInput.h"
#include "nsLib/Vec2.h"
#include "Engine/RenManager.h"

class BaseFunctionalTest : public IUserInput
{
public:
    BaseFunctionalTest();
	virtual ~BaseFunctionalTest() = default;
	virtual bool Init() = 0;
    virtual void Release() = 0;
	virtual void Loop(float deltaTime) = 0;
	virtual void Draw() = 0;

	bool OnPointerUp(float x, float y, int pointerId) override;

	bool OnPointerDown(float x, float y, int pointerId) override;

    bool OnPointerMove(float x, float y, int pointerId) override;

    void OnPointerCancel(int pointerId) override;

    void OnKeyUp(int key) override;

	void OnKeyDown(int key, bool rept) override;

	void OnChar(char ch) override;

	void OnMouseWheel(float delta) override;

protected:
    IRenDevice* const _device;

    virtual nsVec2 WindowToScreen(const nsVec2 &winPos);

    virtual IUserInput* GetInput();
};