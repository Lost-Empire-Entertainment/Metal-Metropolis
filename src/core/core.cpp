//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <array>
#include <filesystem>

#include "log_utils.hpp"

#include "test/test_examples.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_window.hpp"
#include "graphics/ee_scene.hpp"
#include "core/kg_context.hpp"
#include "graphics/kw_window.hpp"
#include "core/kw_input.hpp"
#include "core/kw_core.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using MetalMetropolis::Test::Examples;

using ElypsoEngine::Core::AppConfig;
using ElypsoEngine::Graphics::EngineWindow;
using KalaWindow::Graphics::ProcessWindow;
using KalaGraphics::Core::GraphicsContext;
using KalaWindow::Core::Input;
using KalaWindow::Core::KalaWindowCore;

using std::string;
using std::array;
using std::filesystem::path;

static GraphicsContext* gctx{};
static ProcessWindow* pw{};
static Input* input{};
static path exePath{};

extern const AppConfig ElypsoEngine::Core::appConfig = 
{
    .title = "Metal Metropolis Pre-Release 3",
    .pos = 600
};

void ElypsoEngine::Core::Init()
{
    EngineWindow* ew = EngineWindow::GetRegistry().GetContent(0, false);
    pw = ProcessWindow::GetRegistry().GetContent(ew->GetWindowContextID());
    gctx = GraphicsContext::GetRegistry().GetContent(ew->GetGraphicsContextID());
    input = Input::GetRegistry().GetContent(pw->GetInputID());

    exePath = KalaWindowCore::GetExePath();

    Examples::Test_Popup_And_File_Drag(pw);

    //TODO: fix Y axis in the future, right now its upside-down,
    //so test triangle is also rendered upside down,
    //make sure to put cull mode back to VK_CULL_MODE_BACK_BIT as well once fixed

    Examples::Create_Triangle(
        gctx,
        {
            .pos = { 1, 1, 0 },
            .rot = {},
            .size = { 1, 1, 0 }
        },
        {
            //correct original
            //{.pos = { -0.5f, -0.5f, 0.0f }, .normal = {}, .uv = { 0.0f, 0.0f }},
            //{.pos = {  0.5f, -0.5f, 0.0f }, .normal = {}, .uv = { 1.0f, 0.0f }},
            //{.pos = {  0.0f,  0.5f, 0.0f }, .normal = {}, .uv = { 0.5f, 1.0f }}

            //upside down
            {.pos = { -0.5f,  0.5f, 0.0f }, .normal = {}, .uv = { 0.0f, 1.0f }},
            {.pos = {  0.5f,  0.5f, 0.0f }, .normal = {}, .uv = { 1.0f, 1.0f }},
            {.pos = {  0.0f, -0.5f, 0.0f }, .normal = {}, .uv = { 0.5f, 0.0f }}
        },
        array<path, 2>{
            "files/shaders/rasterized/test_rs_vert.spv",
            "files/shaders/rasterized/test_rs_frag.spv"
        });
}

void ElypsoEngine::Core::EarlyUpdate()
{

}

void ElypsoEngine::Core::FixedUpdate()
{

}

void ElypsoEngine::Core::Update()
{
    string fps = Examples::GetFPS(0.5f);
    if (!fps.empty())
    {
        Log::Print(
            "FPS: " + fps,
            "GAME_CORE",
            LogType::LOG_INFO);
    }

    Examples::Test_VSync_Input(
        gctx,
        input);

    Examples::Test_Create_Notification(input);

    Examples::Test_System_Sound_Input(input);

    Examples::Test_Get_Files(
        input,
        { 
            "*.spv",
            "*.vert",
            "*.frag",
            "*.txt",
            "*.md" },
        path{exePath}.parent_path());

    Examples::Test_Window_Toggles(
        pw,
        input);
}

void ElypsoEngine::Core::LateUpdate()
{
    
}