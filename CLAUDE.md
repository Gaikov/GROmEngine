# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

CMake (minimum 3.10), C++20. Configure and build:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Platform flags:
- `-DNO_NETWORKING=1` — exclude networking subsystem
- `-DWEB_ASM=1` — WebAssembly target
- Sound: `SoundOAL` (default, OpenAL) or `SoundSLES` (Android)

## Tests

Three test targets (not built on Android):
- `nsLib_unit` — math, strings, events, locks
- `Core_unit` — parser, crypto, undo, memory
- `TestsApp` — functional/integration tests for engine systems

Run a specific test binary after building, e.g. `./build/nsLib_unit`.

## Architecture

GROmEngine is a cross-platform 2D/3D game engine targeting Desktop (GLFW/OpenGL) and Android (GLES3/OpenSLES). It is layered bottom-up:

### Layer 1 — nsLib (utility)
Math (`Vec2`, `Vec3`, `Matrix4`), data structures (`EditString`, `FilePath`, `CycleList`), event dispatcher, service locator, abstract factory, logging.

### Layer 2 — Core
Configuration (`nsConfig`), file I/O, serialization/parser, memory pool (`FastMem`), undo/redo (`nsUndoService`), cryptography, command-line args.

### Layer 3 — Platform
- `Platform/Desktop/` — GLFW window creation, `DesktopMain.cpp::main()` entry point
- `Platform/AndroidPlatform/` — Android JNI surface entry point
Both call `nsEngine::Init()` → `IGameApp::Init()` then enter a platform main loop.

### Layer 4 — Rendering
- `RasterGLES3/` — primary renderer (OpenGL 3+ / GLES3, glad loader, GLSL shaders in `shaders/`)
- `RasterGL/` — legacy OpenGL 2 fallback
- `raster_d3d/` — Direct3D stub (placeholder)
All renderers implement `IRenDevice`.

### Layer 5 — Audio
- `SoundBase/` — abstract `ISoundDevice` interface
- `SoundOAL/` — OpenAL implementation (Desktop/Linux)
- `SoundSLES/` — Android OpenSLES implementation
OGG and WAV decoders in each implementation.

### Layer 6 — Engine (top)
`Engine/EngineContext` is the central subsystem. Key subsystems:
- **Display** — `nsVisualObject2d` tree: containers, sprites, text, particles, layouts
- **Input** — multi-user input, soft keyboard
- **Console** — debug variables (`nsVar`) and commands (`nsConfig::RegCmd()`)
- **Particles** — `nsParticlesManager` subsystem
- **Fonts** — `nsFontsManager` subsystem
- **Debug draw** — `nsDebugDrawManager` subsystem
- **Audio** — `nsAudioManager` wraps sound device

### Networking (optional)
Client/server packet architecture with Windows/POSIX socket abstraction. Excluded when `NO_NETWORKING=1`.

## Key Patterns

**`nsSubSystem<T>`** — singleton template used throughout. Access via `T::Shared()`, auto-initialized on first call.

**`IGameApp`** — abstract game interface (`Engine/GameApp.h`). Implement this to create a game:
- `Init()`, `Release()`
- `Loop(frameTime)`, `DrawWorld()`
- `OnActivate()`, `OnPause()`

**Visual object tree** — `nsVisualObject2d` base; parent-child hierarchy via `nsVisualContainer2d`. Objects implement `GetType()`, `Loop()`, `DrawContent()`. New visual types registered via `nsAbstractFactory`.

**`nsEventDispatcher`** — pub/sub for named events; returns handler IDs for unsubscription.

**`nsResourcesCache<T>`** — template cache for textures, fonts, render states, keyed by path.

**Undo/Redo** — command pattern via `nsUndoService` subsystem; used by the GROmEditor layer.

## Code Style

- **Header guards:** `#pragma once` everywhere (не `#ifndef`)
- **Поля класса:** приватные/protected с префиксом `_` (например, `_children`, `_value`); публичные — без префикса (например, `x`, `y`, `id`)
- **Методы:** PascalCase — `GetWidth()`, `AddChild()`, `IsValid()`
- **Классы:** `ns` префикс + PascalCase (`nsVec2`, `nsConfig`); интерфейсы — `I` префикс (`IRenDevice`)
- **Отступы:** 4 пробела
- **Скобки:** открывающая `{` всегда на той же строке — для функций, методов, `if/for/while/class/struct`
- **Указатели/ссылки:** `*` и `&` прижаты к типу: `const nsVec2 &v`, `nsObject *obj`
- **Порядок секций:** `public` → `protected` → `private`
- **`auto`:** использовать везде где возможно
- **Порядок `#include`:** сначала стандартная библиотека, потом локальные заголовки (не критично)
- **Separator-комментарии** перед методами в `.cpp` — опционально, но приветствуются:
  ```cpp
  //---------------------------------------------------------
  // ClassName::MethodName:
  //---------------------------------------------------------
  ```

Старый код в `Core/` и `nsLib/` может содержать `m_` префиксы и `#ifndef` guards — при правке файла приводить к актуальному стилю.

## External Dependencies

OpenGL (glad), OpenAL (openal-soft), OggVorbis, libpng + zlib (in `ThirdParty/ImageDecoder`), GLFW (Desktop), ImGui (`ThirdParty/ImGui`), GTest.
