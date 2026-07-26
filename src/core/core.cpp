//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <chrono>

#include "log_utils.hpp"
#include "key_standards.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_window.hpp"
#include "graphics/ee_scene.hpp"
#include "graphics/kw_window.hpp"
#include "core/kw_input.hpp"
#include "core/kw_core.hpp"
#include "core/kg_context.hpp"
#include "resources/kg_shader.hpp"
#include "resources/kg_mesh.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaKeyStandards::KeyboardButton;

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Core::AppConfig;
using ElypsoEngine::Graphics::EngineWindow;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Core::Input;
using KalaWindow::Core::KalaWindowCore;
using KalaGraphics::Core::GraphicsContext;
using KalaGraphics::Core::VSyncState;
using KalaGraphics::Resources::Shader;
using KalaGraphics::Resources::Mesh;
using KalaGraphics::Resources::Transform;
using KalaGraphics::Resources::Vertex;

using std::string;
using std::chrono::time_point;
using std::chrono::steady_clock;
using std::chrono::seconds;
using std::chrono::duration;
using std::format;

Input* input{};
GraphicsContext* gctx{};

static string GetFPS(f64 secondsToWait)
{
    static f64 cachedFPS = EngineCore::GetCurrentFPS();
    static time_point start = steady_clock::now();

    if (steady_clock::now() - start > duration<f64>(secondsToWait))
    {
        cachedFPS = EngineCore::GetCurrentFPS();
        start = steady_clock::now();
    }
    
    return format("{:.2f}", cachedFPS);
}

extern const AppConfig ElypsoEngine::Core::appConfig = 
{
    .title = "Metal Metropolis Pre-Release 2",
    .pos = 600
};

void ElypsoEngine::Core::Init()
{
    EngineWindow* ew = EngineWindow::GetRegistry().runtimeContent[0];
    ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(ew->GetWindowContextID());
    gctx = GraphicsContext::GetRegistry().GetContent(ew->GetGraphicsContextID());
    input = Input::GetRegistry().GetContent(pw->GetInputID());

    //sync ids before generating shader
    EngineCore::SyncID();

    Shader* shader = Shader::Initialize(
        gctx->GetID(),
        "shader-test",
        {
            .shader_vert = "files/shaders/rasterized/test_rs_vert.spv",
            .shader_frag = "files/shaders/rasterized/test_rs_frag.spv"
        });

    if (!shader)
    {
        KalaWindowCore::ForceClose(
            "metal metropolis error",
            "failed to init shader!");
    }

    Transform transform =
    {
        .pos = { 1, 1, 0 },
        .rot = {},
        .size = { 1, 1, 0 }
    };

    //TODO: fix Y axis in the future, right now its upside-down,
    //so test triangle is also rendered upside down,
    //make sure to put cull mode back to VK_CULL_MODE_BACK_BIT as well once fixed

    vector<Vertex> testTriangle = 
    {
        //correct original
        //{.pos = { -0.5f, -0.5f, 0.0f }, .normal = {}, .uv = { 0.0f, 0.0f }},
        //{.pos = {  0.5f, -0.5f, 0.0f }, .normal = {}, .uv = { 1.0f, 0.0f }},
        //{.pos = {  0.0f,  0.5f, 0.0f }, .normal = {}, .uv = { 0.5f, 1.0f }}

        //upside down
        {.pos = { -0.5f,  0.5f, 0.0f }, .normal = {}, .uv = { 0.0f, 1.0f }},
        {.pos = {  0.5f,  0.5f, 0.0f }, .normal = {}, .uv = { 1.0f, 1.0f }},
        {.pos = {  0.0f, -0.5f, 0.0f }, .normal = {}, .uv = { 0.5f, 0.0f }}
    };

    //sync ids before generating mesh
    EngineCore::SyncID();

    Mesh* mesh = Mesh::Initialize(
        "mesh-test",
        true,
        shader->GetID(),
        std::move(transform),
        std::move(testTriangle),
        {});

    if (!mesh)
    {
        KalaWindowCore::ForceClose(
            "metal metropolis error",
            "failed to init mesh!");
    }
}

void ElypsoEngine::Core::FixedUpdate()
{
    /*
    Log::Print(
        "This is a fixed update...",
        "MM_MAIN",
        LogType::LOG_DEBUG);
    */
}

void ElypsoEngine::Core::Update()
{
    //Log::Print("fps: " + GetFPS(0.5));

    if (input->IsKeyPressed(KeyboardButton::K_1))
    {
        Log::Print("@@@@@ set vsync state to off");
        gctx->SetVSyncState(VSyncState::VSYNC_OFF);
    }
    if (input->IsKeyPressed(KeyboardButton::K_2))
    {
        Log::Print("@@@@@ set vsync state to adaptive");
        gctx->SetVSyncState(VSyncState::VSYNC_ON_ADAPTIVE);
    }
    if (input->IsKeyPressed(KeyboardButton::K_3))
    {
        Log::Print("@@@@@ set vsync state to triple buffered");
        gctx->SetVSyncState(VSyncState::VSYNC_ON_TRIPLE_BUFFERED);
    }
}