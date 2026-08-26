//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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
#include "core/kg_viewport.hpp"
#include "resources/kg_shader.hpp"
#include "resources/kg_mesh.hpp"
#include "resources/kg_texture.hpp"
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
using KalaGraphics::Core::Viewport;
using KalaGraphics::Core::ViewportType;
using KalaGraphics::Resources::Vertex;
using KalaGraphics::Resources::Shader;
using KalaGraphics::Resources::Mesh;
using KalaGraphics::Resources::Texture;
using KalaGraphics::Resources::TextureData;
using KalaGraphics::Resources::TextureFilterMode;
using KalaGraphics::Resources::FALLBACK_TEXTURE;
using KalaGraphics::Resources::Camera;
using KalaGraphics::Resources::CameraType;
using KalaGraphics::Import::ImportTexture;

using std::string;
using std::filesystem::path;

static GraphicsContext* gctx{};
static Viewport* vp{};
static ProcessWindow* pw{};
static Input* input{};

static Shader* shader3D{};
static Mesh* mesh{};
static Texture* tex{};
static Camera* cam{};

static path exePath{};

static const path shader_unlit_vert = path("files") / "shaders" / "unlit_vert.spv";
static const path shader_unlit_frag = path("files") / "shaders" / "unlit_frag.spv";

static const path shader_ui_rect_vert = path("files") / "shaders" / "ui_rect_vert.spv";
static const path shader_ui_rect_frag = path("files") / "shaders" / "ui_rect_frag.spv";

extern const AppConfig ElypsoEngine::Core::appConfig = 
{
    .title = "Metal Metropolis Pre-Release 5",
    .pos = 600
};

void ElypsoEngine::Core::Init()
{
    EngineWindow* ew{};
    string err = EngineWindow::GetRegistry().GetContent(0, ew, false);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get engine window '0'! Reason: " + err);
    }

    err = ProcessWindow::GetRegistry().GetContent(ew->GetWindowContextID(), pw);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get process window from engine window '" + to_string(ew->GetID()) + "'! Reason: " + err);
    }

    pw->SetMinSize({800, 600});

    err = GraphicsContext::GetRegistry().GetContent(ew->GetGraphicsContextID(), gctx);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get graphics context from process window '" + to_string(pw->GetID()) + "'! Reason: " + err);
    }

    err = Viewport::GetRegistry().GetContent(gctx->GetRootViewportID(), vp);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get root viewport from graphics context '" + to_string(gctx->GetID()) + "'! Reason: " + err);
    }

    err = Shader::GetRegistry().GetContent(vp->GetPrimary3DShaderID(), shader3D);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get primary 3D shader from viewport '" + to_string(vp->GetID()) + "'! Reason: " + err);
    }

    err = Camera::GetRegistry().GetContent(vp->GetPrimary3DCameraID(), cam);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get primary 3D camera from viewport '" + to_string(vp->GetID()) + "'! Reason: " + err);
    }
    cam->SetSensitivityMultiplier(0.175f);
    cam->SetSpeedMultiplier(7.5f);

    err = Input::GetRegistry().GetContent(pw->GetInputID(), input);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get input from engine windows process window '" + to_string(pw->GetID()) + "'! Reason: " + err);
    }

    exePath = KalaWindowCore::GetExePath();

    Examples::Test_Popup_And_File_Drag(pw);

    /*
    shader = Examples::Test_Create_Shader(
        gctx,
        array<path, 2>{
            "files/shaders/unlit_vert.spv",
            "files/shaders/unlit_frag.spv"
        });
    */

    TextureData tdata =
    {
        .pixelData = vector<u8>(
                FALLBACK_TEXTURE.begin(), 
                FALLBACK_TEXTURE.end()),
        .filterMode = TextureFilterMode::FILTER_NEAREST,
        .size = 16
    };
    tex = Examples::Test_Create_Texture(
        shader3D,
        std::move(tdata));

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
        shader3D,
        tex,
        {
            .pos = { 0, 0, 0 },
            .rot = {},
            .size = { 1, 1, 1 }
        },
        vector<Vertex>{vertices},
        vector<u32>{indices});

    /*
    ImportTexture* it = ImportTexture::Initialize(exePath.parent_path() / "files/textures/test.png");
    if (!it)
    {
        KalaWindowCore::ForceClose(
            "test",
            "import texture force close");
    }
    */

    //sync to ensure viewport gets the highest id
    EngineCore::SyncID();

    Viewport* secondVP = Viewport::Initialize(gctx->GetID());

    //sync to ensure 3D shader gets the highest id
    EngineCore::SyncID();

    Shader* unlit = Shader::Initialize(
        secondVP->GetID(),
        false,
        path(shader_unlit_vert),
        path(shader_unlit_frag));

    if (!unlit)
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to create new unlit shader!");
    }

    //sync to ensure 2D shader gets the highest id
    EngineCore::SyncID();

    Shader* rect = Shader::Initialize(
        secondVP->GetID(),
        true,
        path(shader_ui_rect_vert),
        path(shader_ui_rect_frag));

    if (!rect)
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to create new rect shader!");
    }

    //sync to ensure 3D cam gets the highest id
    EngineCore::SyncID();

    Camera* new3DCam = Camera::Initialize(
        unlit->GetID(),
        CameraType::CAM_PERSPECTIVE);

    if (!new3DCam)
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to create new 3D camera!");
    }

    //sync to ensure 2D cam gets the highest id
    EngineCore::SyncID();

    Camera* new2DCam = Camera::Initialize(
        rect->GetID(),
        CameraType::CAM_ORTHOGRAPHIC);

    if (!new2DCam)
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to create new 2D camera!");
    }

    secondVP->SetDynamicResizeState(false);
    secondVP->SetType(ViewportType::VP_FILL);
    secondVP->SetSize(250);
    secondVP->SetOffset(100);
    secondVP->SetBackgroundColor(1);

    vp->SetType(ViewportType::VP_CENTER);
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

    Examples::Test_Texture_Filter_Mode(
        input,
        tex);

    Examples::Test_Camera_Toggle(input);

    Examples::Test_Camera_Move(
        input,
        cam,
        EngineCore::GetDeltaTime());
}

void ElypsoEngine::Core::LateUpdate()
{
    
}