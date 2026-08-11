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
#include "graphics/kw_window.hpp"
#include "core/kw_input.hpp"
#include "core/kw_core.hpp"
#include "core/kg_context.hpp"
#include "resources/kg_shader.hpp"
#include "resources/kg_mesh.hpp"
#include "resources/kg_camera.hpp"
#include "import/kg_import_texture.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using MetalMetropolis::Test::Examples;

using ElypsoEngine::Core::AppConfig;
using ElypsoEngine::Graphics::EngineWindow;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Core::Input;
using KalaWindow::Core::KalaWindowCore;
using KalaGraphics::Core::GraphicsContext;
using KalaGraphics::Resources::Vertex;
using KalaGraphics::Resources::Shader;
using KalaGraphics::Resources::Mesh;
using KalaGraphics::Resources::Camera;
using KalaGraphics::Import::ImportTexture;

using std::string;
using std::array;
using std::filesystem::path;

static GraphicsContext* gctx{};
static ProcessWindow* pw{};
static Input* input{};

static Shader* shader{};
static Mesh* mesh{};
static Camera* cam{};

static path exePath{};

extern const AppConfig ElypsoEngine::Core::appConfig = 
{
    .title = "Metal Metropolis Pre-Release 4",
    .pos = 600
};

void ElypsoEngine::Core::Init()
{
    EngineWindow* ew = EngineWindow::GetRegistry().GetContent(0, false);
    pw = ProcessWindow::GetRegistry().GetContent(ew->GetWindowContextID());
    pw->SetMinSize({800, 600});

    gctx = GraphicsContext::GetRegistry().GetContent(ew->GetGraphicsContextID());
    input = Input::GetRegistry().GetContent(pw->GetInputID());

    exePath = KalaWindowCore::GetExePath();

    Examples::Test_Popup_And_File_Drag(pw);

    shader = Examples::Test_Create_Shader(
        gctx,
        array<path, 2>{
            "files/shaders/rasterized/mesh_basic_vert.spv",
            "files/shaders/rasterized/mesh_basic_frag.spv"
        });

    vector<Vertex> vertices =
    {
        //front (+Z)
        {.pos = { -1.0f, -1.0f,  1.0f }, .normal = { 0.0f, 0.0f, 1.0f }, .uv = { 0.0f, 0.0f }},
        {.pos = {  1.0f, -1.0f,  1.0f }, .normal = { 0.0f, 0.0f, 1.0f }, .uv = { 1.0f, 0.0f }},
        {.pos = {  1.0f,  1.0f,  1.0f }, .normal = { 0.0f, 0.0f, 1.0f }, .uv = { 1.0f, 1.0f }},
        {.pos = { -1.0f,  1.0f,  1.0f }, .normal = { 0.0f, 0.0f, 1.0f }, .uv = { 0.0f, 1.0f }},

        //back (-Z)
        {.pos = {  1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f }},
        {.pos = { -1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f }},
        {.pos = { -1.0f,  1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f }},
        {.pos = {  1.0f,  1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f }},

        //left (-X)
        {.pos = { -1.0f, -1.0f, -1.0f }, .normal = { -1.0f, 0.0f, 0.0f }, .uv = { 0.0f, 0.0f }},
        {.pos = { -1.0f, -1.0f,  1.0f }, .normal = { -1.0f, 0.0f, 0.0f }, .uv = { 1.0f, 0.0f }},
        {.pos = { -1.0f,  1.0f,  1.0f }, .normal = { -1.0f, 0.0f, 0.0f }, .uv = { 1.0f, 1.0f }},
        {.pos = { -1.0f,  1.0f, -1.0f }, .normal = { -1.0f, 0.0f, 0.0f }, .uv = { 0.0f, 1.0f }},

        //right (+X)
        {.pos = {  1.0f, -1.0f,  1.0f }, .normal = { 1.0f, 0.0f, 0.0f }, .uv = { 0.0f, 0.0f }},
        {.pos = {  1.0f, -1.0f, -1.0f }, .normal = { 1.0f, 0.0f, 0.0f }, .uv = { 1.0f, 0.0f }},
        {.pos = {  1.0f,  1.0f, -1.0f }, .normal = { 1.0f, 0.0f, 0.0f }, .uv = { 1.0f, 1.0f }},
        {.pos = {  1.0f,  1.0f,  1.0f }, .normal = { 1.0f, 0.0f, 0.0f }, .uv = { 0.0f, 1.0f }},

        //top (+Y)
        {.pos = { -1.0f,  1.0f,  1.0f }, .normal = { 0.0f, 1.0f, 0.0f }, .uv = { 0.0f, 0.0f }},
        {.pos = {  1.0f,  1.0f,  1.0f }, .normal = { 0.0f, 1.0f, 0.0f }, .uv = { 1.0f, 0.0f }},
        {.pos = {  1.0f,  1.0f, -1.0f }, .normal = { 0.0f, 1.0f, 0.0f }, .uv = { 1.0f, 1.0f }},
        {.pos = { -1.0f,  1.0f, -1.0f }, .normal = { 0.0f, 1.0f, 0.0f }, .uv = { 0.0f, 1.0f }},

        //bottom (-Y)
        {.pos = { -1.0f, -1.0f, -1.0f }, .normal = { 0.0f, -1.0f, 0.0f }, .uv = { 0.0f, 0.0f }},
        {.pos = {  1.0f, -1.0f, -1.0f }, .normal = { 0.0f, -1.0f, 0.0f }, .uv = { 1.0f, 0.0f }},
        {.pos = {  1.0f, -1.0f,  1.0f }, .normal = { 0.0f, -1.0f, 0.0f }, .uv = { 1.0f, 1.0f }},
        {.pos = { -1.0f, -1.0f,  1.0f }, .normal = { 0.0f, -1.0f, 0.0f }, .uv = { 0.0f, 1.0f }},
    };

    vector<u32> indices =
    {
        0,   2,   1,   0,   3,   2,   //front
        4,   6,   5,   4,   7,  6,  //back
        8,  10, 9,  8,  11, 10, //left
        12, 14, 13, 12, 15, 14, //right
        16, 18, 17, 16, 19, 18, //top
        20, 22, 21, 20, 23, 22, //bottom
    };

    mesh = Examples::Test_Create_Mesh(
        shader,
        {
            .pos = { 0, 0, 0 },
            .rot = {},
            .size = { 1, 1, 1 }
        },
        vector<Vertex>{vertices},
        vector<u32>{indices});

    cam = Examples::Test_Create_Camera(
        gctx,
        shader);
    cam->SetSensitivityMultiplier(0.175f);
    cam->SetSpeedMultiplier(7.5f);

    /*
    ImportTexture* it = ImportTexture::Initialize(exePath.parent_path() / "files/textures/test.png");
    if (!it)
    {
        KalaWindowCore::ForceClose(
            "test",
            "import texture force close");
    }
    */
}

void ElypsoEngine::Core::EarlyUpdate()
{

}

void ElypsoEngine::Core::FixedUpdate()
{

}

void ElypsoEngine::Core::Update()
{
    string fps = Examples::Test_Get_FPS(0.5f);
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

    Examples::Test_Camera_Toggle(input);

    Examples::Test_Camera_Move(
        input,
        cam,
        EngineCore::GetDeltaTime());
}

void ElypsoEngine::Core::LateUpdate()
{
    
}