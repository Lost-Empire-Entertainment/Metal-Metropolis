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
#include "objects/models/kg_model_primitive.hpp"

using KalaHeaders::KalaMath::vec3;

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::Render;
using ElypsoEngine::Graphics::EngineWindow;
using ElypsoEngine::Entity::Object;
using KalaGraphics::Object::CubeDetails;

using std::string_view;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;
using std::filesystem::path;

constexpr string_view programName = "Metal Metropolis Pre-Release 0003";

int main()
{ 
    EngineCore::Initialize(programName);

    EngineWindow* ew = EngineWindow::Initialize(programName);

    Object::Initialize(
        "cube",
        path("files") / "shaders",
        ew->GetID(),
        {},
        {},
        vec3(1),
        CubeDetails{});

    while (true)
    {
        Render::Update();

        sleep_for(milliseconds(1));
    }
}