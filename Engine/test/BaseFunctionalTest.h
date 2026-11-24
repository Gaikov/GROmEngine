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

	bool OnPointerUp(const nsPointerData &pointer) override;

	bool OnPointerDown(const nsPointerData &pointer) override;

    bool OnPointerMove(const nsPointerData &pointer) override;

    void OnPointerCancel(const nsPointerData &pointer) override;

    void OnKeyUp(int key, int mods) override;

	void OnKeyDown(int key, bool rept, int mods) override;

	void OnChar(char ch) override;

    bool OnMouseWheel(float delta) override;

protected:
    IRenDevice* const _device;

    virtual nsVec2 WindowToScreen(const nsVec2 &winPos);

    virtual IUserInput* GetInput();
};