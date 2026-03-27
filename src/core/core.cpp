//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "core/ee_core.hpp"
#include "graphics/ee_window.hpp"

#include "core/core.hpp"

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::EngineWindow;

namespace MetalMetropolis::Core
{
    void GameCore::Initialize()
    {
        EngineCore::Initialize("1234");

        EngineWindow::Initialize();

        EngineCore::Update();
    }
}