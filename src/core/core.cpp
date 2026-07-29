//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "log_utils.hpp"

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

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Core::AppConfig;
using ElypsoEngine::Graphics::EngineWindow;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Core::Input;
using KalaWindow::Core::KalaWindowCore;
using KalaGraphics::Core::GraphicsContext;
using KalaGraphics::Resources::Shader;
using KalaGraphics::Resources::Mesh;
using KalaGraphics::Resources::Transform;
using KalaGraphics::Resources::Vertex;

using std::string;

Input* input{};
GraphicsContext* gctx{};

extern const AppConfig ElypsoEngine::Core::appConfig = 
{
    .title = "Metal Metropolis Pre-Release 2",
    .pos = 600
};

void ElypsoEngine::Core::Init()
{
    EngineWindow* ew = EngineWindow::GetRegistry().GetContent(0, false);
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
            "Game core error",
            "Failed to initialize shader 'shader-test'!");
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
        gctx->GetID(),
        shader->GetID(),
        std::move(transform),
        std::move(testTriangle),
        {});

    if (!mesh)
    {
        KalaWindowCore::ForceClose(
            "Game core error",
            "Failed to initialize mesh 'mesh-test'!");
    }
}

void ElypsoEngine::Core::FixedUpdate()
{

}

void ElypsoEngine::Core::Update()
{

}