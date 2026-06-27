//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string_view>
#include <thread>
#include <chrono>

#include "math_utils.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_render.hpp"
#include "graphics/ee_window.hpp"
#include "core/kw_core.hpp"
#include "graphics/kw_window.hpp"

using KalaHeaders::KalaMath::vec3;

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::Render;
using ElypsoEngine::Graphics::EngineWindow;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::ProcessWindow;

using std::string_view;
using std::to_string;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;

constexpr string_view gameTitle = "Metal Metropolis Pre-Release 1";

int main()
{ 
    EngineCore::Initialize(gameTitle);
    EngineWindow* ewin = EngineWindow::Initialize(
        gameTitle,
        100,
        { 800, 600 });

    ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(ewin->GetWindowContextID());
    if (!pw)
    {
        KalaWindowCore::ForceClose(
            "Window error",
            "Failed to get process window from ID '" + to_string(ewin->GetWindowContextID()) + "'");
    }

    pw->SetMinSize({800, 600});

    while (true)
    {
        Render::Update();

        sleep_for(milliseconds(1));
    }
}