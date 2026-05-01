//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string_view>
#include <thread>
#include <chrono>
#include <filesystem>

#include "math_utils.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_render.hpp"
#include "graphics/ee_window.hpp"
#include "entity/ee_entity_object.hpp"
#include "graphics/models/kg_model_primitive.hpp"
#include "core/kw_input.hpp"
#include "vulkan/kw_vulkan.hpp"
#include "graphics/kg_vulkan.hpp"

using KalaHeaders::KalaMath::vec3;

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::Render;
using ElypsoEngine::Graphics::EngineWindow;
using ElypsoEngine::Entity::Object;
using KalaGraphics::Graphics::CubeDetails;
using KalaWindow::Core::Input;
using KalaWindow::Vulkan::Vulkan_Global;
using KalaGraphics::Graphics::Vulkan_Core;

using std::string_view;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;
using std::filesystem::path;

constexpr string_view gameTitle = "Metal Metropolis Pre-Release 0003";

int main()
{ 
    EngineCore::Initialize(gameTitle);
    EngineWindow* ew = EngineWindow::Initialize(gameTitle);

    //Input::SetVerboseLoggingState(true);
    //Vulkan_Global::SetVerboseLoggingState(true);
    Vulkan_Core::SetVerboseLoggingState(true);

    /*
    Object::Initialize(
        "cube",
        path("files") / "shaders",
        ew->GetID(),
        {},
        {},
        vec3(1),
        CubeDetails{});
    */

    while (true)
    {
        Render::Update();

        sleep_for(milliseconds(1));
    }
}