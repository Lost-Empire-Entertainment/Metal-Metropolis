//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string_view>
#include <thread>
#include <chrono>

#include "core/ee_core.hpp"
#include "graphics/ee_render.hpp"
#include "graphics/ee_window.hpp"

#include "core/core.hpp"

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::Render;
using ElypsoEngine::Graphics::EngineWindow;

using std::string_view;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;

constexpr string_view programName = "Metal Metropolis";

namespace MetalMetropolis::Core
{
    void GameCore::Initialize()
    {
        EngineCore::Initialize(programName);

        EngineWindow::Initialize(programName);

        while (true)
        {
            Render::Update();

            sleep_for(milliseconds(1));
        }
    }
}