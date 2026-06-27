//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string_view>
#include <thread>
#include <chrono>

#include "math_utils.hpp"

#include "test/crash_test.hpp"
#include "core/ee_core.hpp"
#include "graphics/ee_render.hpp"
#include "graphics/ee_window.hpp"

using KalaHeaders::KalaMath::vec3;

using MetalMetropolis::Test::AccessViolationType;
using MetalMetropolis::Test::Crash_AccessViolation;
using MetalMetropolis::Test::Crash_StackOverflow;
using MetalMetropolis::Test::Crash_DivideByZero;

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::Render;
using ElypsoEngine::Graphics::EngineWindow;

using std::string_view;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;

constexpr string_view gameTitle = "Metal Metropolis Pre-Release 0004";

int main()
{ 
    EngineCore::Initialize(gameTitle);
    EngineWindow::Initialize(gameTitle);

    while (true)
    {
        Render::Update();

        sleep_for(milliseconds(1));
    }
}