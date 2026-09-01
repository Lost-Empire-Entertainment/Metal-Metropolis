//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>

#include "log_utils.hpp"
#include "math_utils.hpp"

#include "test/test_examples.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_window.hpp"
#include "graphics/ee_scene.hpp"
#include "graphics/kw_window.hpp"
#include "core/kw_input.hpp"
#include "core/kw_core.hpp"
#include "core/kg_context.hpp"
#include "core/kg_viewport.hpp"
#include "core/kg_hit_test.hpp"
#include "resources/kg_shader.hpp"
#include "resources/kg_mesh.hpp"
#include "resources/kg_texture.hpp"
#include "resources/kg_camera.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaMath::vec3;
using KalaHeaders::KalaMath::epsilon;
using KalaHeaders::KalaMath::Transform3D;

using MetalMetropolis::Test::Examples;

using ElypsoEngine::Core::AppConfig;
using ElypsoEngine::Graphics::EngineWindow;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Core::Input;
using KalaWindow::Core::KalaWindowCore;
using KalaGraphics::Core::GraphicsContext;
using KalaGraphics::Core::Viewport;
using KalaGraphics::Core::ViewportType;
using KalaGraphics::Core::HitTest;
using KalaGraphics::Resources::AnchorPosition;
using KalaGraphics::Resources::Shader;
using KalaGraphics::Resources::Mesh;
using KalaGraphics::Resources::Mesh_Cube;
using KalaGraphics::Resources::Mesh_Pyramid;
using KalaGraphics::Resources::Mesh_Sphere;
using KalaGraphics::Resources::Texture;
using KalaGraphics::Resources::TextureData;
using KalaGraphics::Resources::TextureFilterMode;
using KalaGraphics::Resources::FALLBACK_TEXTURE;
using KalaGraphics::Resources::Camera;
using KalaGraphics::Resources::CameraType;

using std::string;
using std::filesystem::path;

static GraphicsContext* gctx{};
static Viewport* vp{};
static Viewport* vp2{};
static HitTest* ht{};

static ProcessWindow* pw{};
static Input* input{};

static Shader* shader3D{};
static Shader* shader2D{};

static Texture* tex{};
static Camera* cam3D{};

static Mesh* mesh3D_cube{};
static Mesh* mesh3D_pyramid{};
static Mesh* mesh3D_sphere{};
static Mesh* mesh2D_rect{};

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
    //print all user system info for now always at the very top

    Log::Print(KalaWindowCore::GetCPUInfoString());
    Log::Print(" ");
    Log::Print(KalaWindowCore::GetGPUInfoString());
    Log::Print(" ");
    Log::Print(KalaWindowCore::GetRAMInfoString(true));
    Log::Print(" ");
    Log::Print(KalaWindowCore::GetOSInfoString());
    Log::Print(" ");

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

    err = HitTest::GetRegistry().GetContent(gctx->GetHitTestID(), ht);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get hit test from graphics context '" + to_string(gctx->GetID()) + "'! Reason: " + err);
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

    err = Shader::GetRegistry().GetContent(vp->GetPrimary2DShaderID(), shader2D);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get primary 2D shader from viewport '" + to_string(vp->GetID()) + "'! Reason: " + err);
    }

    err = Camera::GetRegistry().GetContent(vp->GetPrimary3DCameraID(), cam3D);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get primary 3D camera from viewport '" + to_string(vp->GetID()) + "'! Reason: " + err);
    }
    cam3D->SetSensitivityMultiplier(0.175f);
    cam3D->SetSpeedMultiplier(7.5f);

    Transform3D& c3t = cam3D->GetTransform();

    err = Input::GetRegistry().GetContent(pw->GetInputID(), input);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get input from engine windows process window '" + to_string(pw->GetID()) + "'! Reason: " + err);
    }

    exePath = KalaWindowCore::GetExePath();

    Examples::Test_Popup_And_File_Drag(pw);

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

    //
    // CREATE 3D CUBE
    //

    mesh3D_cube = Examples::Test_Create_Mesh(
        shader3D,
        tex,
        Mesh::GenerateMeshData(Mesh_Cube{.edgeCount = 4}));

    Transform3D& m3ct = mesh3D_cube->GetTransform();
    m3ct.setpos(
        c3t.getdirfront() * 2.0f 
        - c3t.getdirright() * 2.0f);

    //
    // CREATE 3D PYRAMID
    //

    mesh3D_pyramid = Examples::Test_Create_Mesh(
        shader3D,
        tex,
        Mesh::GenerateMeshData(Mesh_Pyramid{.edgeCount = 4}));

    Transform3D& m3pt = mesh3D_pyramid->GetTransform();
    m3pt.setpos(c3t.getdirfront() * 2.0f);

    //
    // CREATE 3D SPHERE
    //

    mesh3D_sphere = Examples::Test_Create_Mesh(
        shader3D,
        tex,
        Mesh::GenerateMeshData(Mesh_Sphere{}));

    Transform3D& m3st = mesh3D_sphere->GetTransform();
    m3st.setpos(
        c3t.getdirfront() * 2.0f 
        + c3t.getdirright() * 2.0f);

    //
    // CREATE 2D QUAD
    //

    mesh2D_rect = Examples::Test_Create_Mesh(
        shader2D,
        tex,
        Mesh::GenerateMeshData());

    Transform3D& m2rt = mesh2D_rect->GetTransform();
    m2rt.setsize({ 100.0f, 100.0f, epsilon });

    //
    // CREATE SECOND VIEWPORT
    //

    //sync to ensure viewport gets the highest id
    EngineCore::SyncID();

    vp2 = Viewport::Initialize(gctx->GetID());

    //sync to ensure 3D shader gets the highest id
    EngineCore::SyncID();

    Shader* unlit = Shader::Initialize(
        vp2->GetID(),
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
        vp2->GetID(),
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

    vp2->SetDynamicResizeState(false);
    vp2->SetType(ViewportType::VP_FILL);
    vp2->SetSize(250);
    vp2->SetOffset(100);
    vp2->SetBackgroundColor(1);
    vp2->SetVisibleState(false);

    vp->SetType(ViewportType::VP_FIT);

    mesh2D_rect->SetViewportAnchorPosition(AnchorPosition::P_TOP_LEFT);
    mesh2D_rect->SetLocalAnchorPosition(AnchorPosition::P_TOP_LEFT);
}

void ElypsoEngine::Core::EarlyUpdate()
{

}

void ElypsoEngine::Core::FixedUpdate()
{

}

void ElypsoEngine::Core::Update()
{
    Examples::Test_Mesh_Toggle_Recreate_Target(input);
    Examples::Test_Mesh_Recreate_Cube_On_Mouse_Actions(
        input,
        mesh3D_cube);
    Examples::Test_Mesh_Recreate_Pyramid_On_Mouse_Actions(
        input,
        mesh3D_pyramid);
    Examples::Test_Mesh_Recreate_Sphere_On_Mouse_Actions(
        input,
        mesh3D_sphere);

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
        cam3D,
        EngineCore::GetDeltaTime());
}

void ElypsoEngine::Core::LateUpdate()
{
    
}