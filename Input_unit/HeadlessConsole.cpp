// Test double for the console gate used by the production Input.cpp.
// No renderer, native window, resource loading or console UI is initialized.
#include "Engine/console/Console.h"

nsConsole::nsConsole() : _input(128), _scrollLines(0), m_tex(nullptr),
    _renState(nullptr), tex_offs{0, 0}, m_bActive(false), _buffer(1024) {}
nsConsole::~nsConsole() = default;
bool nsConsole::OnInit() { return true; }
void nsConsole::OnRelease() {}
bool nsConsole::IsActive() { return m_bActive; }
void nsConsole::Activate(bool active) { m_bActive = active; }
void nsConsole::Toggle() { Activate(!m_bActive); }
bool nsConsole::OnMouseWheel(float) { return true; }
void nsConsole::OnKeyDown(int, bool, int) {}
void nsConsole::OnChar(char) {}
void nsConsole::OnPointerCancel(int) {}
void nsConsole::LogPrint(LogLevel, const char*) {}
