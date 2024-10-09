//
// Created by Roman on 9/5/2024.
//

#include "EngineContext.h"
#include "Core/sys.h"
#include "RenManager.h"
#include "GameApp.h"
#include "Console.h"
#include "input/SoftKeyboard.h"

bool nsEngineContext::OnInit() {
    nsSubSystem::OnInit();
    _root = new nsGroupLayout();
    if (!nsSoftKeyboard::Init()) {
        return false;
    }
    auto kb = nsSoftKeyboard::Shared();
    kb->SetHandler(this);
    _root->AddChild(kb->GetRoot());

    _rsGUI = nsRenDevice::Shared()->Device()->StateLoad( "scripts/rs/gui.txt" );
    if ( !_rsGUI )
    {
        Sys_FatalError( "Can't load GUI render state!\n" );
        return false;
    }

    _engineInput._pointerTransform = &_nativeOrtho;
    return true;
}

void nsEngineContext::OnRelease() {
    nsRenDevice::Shared()->Device()->StateRelease(_rsGUI);
    nsSoftKeyboard::Release();
    delete _root;
    nsSubSystem::OnRelease();
}

IUserInput *nsEngineContext::GetActiveInput() {
    _engineInput.ClearInputs();
    _input.ClearInputs();
    _input.AddInput(&_engineInput);

    auto kb = nsSoftKeyboard::Shared();
    if (kb->IsActive()) {
        _engineInput.AddInput(_root);
    }

    auto console = nsConsole::Shared();
    if (console->IsActive()) {
        _engineInput.AddInput(console);
    } else {
        auto gameInput = App_GetGame()->GetUserInput();
        if (gameInput) {
            _input.AddInput(gameInput);
        }
    }

    return &_input;
}

void nsEngineContext::OnKeyUp(int key) {
    GetActiveInput()->OnKeyUp(key);
}

void nsEngineContext::OnKeyDown(int key, bool rept) {
    GetActiveInput()->OnKeyDown(key, rept);
}

void nsEngineContext::OnChar(char ch) {
    GetActiveInput()->OnChar(ch);
}


