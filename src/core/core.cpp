//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>
#include <array>

#include "log_utils.hpp"
#include "math_utils.hpp"

#include "test/test_examples.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_window.hpp"
#include "graphics/ee_scene.hpp"
#include "graphics/kw_window_global.hpp"
#include "graphics/kw_window.hpp"
#include "core/kw_input.hpp"
#include "core/kw_core.hpp"
#include "core/kg_context.hpp"
#include "core/kg_viewport.hpp"
#include "core/kg_hit_test.hpp"
#include "core/kg_shader.hpp"
#include "resources/kg_mesh.hpp"
#include "resources/kg_texture.hpp"
#include "resources/kg_camera.hpp"
#include "import/kg_import_font.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaMath::epsilon;
using KalaHeaders::KalaMath::vec4;
using KalaHeaders::KalaMath::vec3;
using KalaHeaders::KalaMath::Transform3D;
using KalaHeaders::KalaMath::Transform2D;

using KalaHeaders::KalaKeyStandards::KeyboardButton;
using KalaHeaders::KalaKeyStandards::MouseButton;

using MetalMetropolis::Test::Examples;

using ElypsoEngine::Core::AppConfig;
using ElypsoEngine::Graphics::EngineWindow;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::FileType;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Core::Input;
using KalaWindow::Core::KalaWindowCore;
using KalaGraphics::Core::GraphicsContext;
using KalaGraphics::Core::RootShaderTarget;
using KalaGraphics::Core::Viewport;
using KalaGraphics::Core::ViewportType;
using KalaGraphics::Core::Shader;
using KalaGraphics::Core::HitTest;
using KalaGraphics::Resources::AnchorPosition;
using KalaGraphics::Resources::Mesh;
using KalaGraphics::Resources::Mesh_Cube;
using KalaGraphics::Resources::Mesh_Pyramid;
using KalaGraphics::Resources::Mesh_Sphere;
using KalaGraphics::Resources::Vertex;
using KalaGraphics::Resources::Texture;
using KalaGraphics::Resources::TextureData;
using KalaGraphics::Resources::TexturePixelFormat;
using KalaGraphics::Resources::TextureFilterMode;
using KalaGraphics::Resources::TextureWrapMode;
using KalaGraphics::Resources::FALLBACK_TEXTURE;
using KalaGraphics::Resources::Camera;
using KalaGraphics::Import::ImportFont;

using std::string;
using std::filesystem::path;
using std::vector;
using std::array;
using std::pair;

static path exePath{};

/*
static constexpr array<vec4, 6> colors
{{
    { 1.00f, 1.00f, 1.00f, 1.00f },  //plain white
    { 1.00f, 0.25f, 0.20f, 0.05f },  //coral red
    { 0.20f, 0.75f, 1.00f, 1.00f },  //sky blue
    { 0.35f, 1.00f, 0.40f, 0.025f }, //lime green
    { 0.85f, 0.30f, 1.00f, 1.00f },  //vivid purple
    { 1.00f, 0.75f, 0.15f, 0.75f }   //golden yellow
}};
*/

//static ImportFont* font{};

static EngineWindow* ew1{};
static GraphicsContext* ew1_gctx{};
static Viewport* ew1_gctx_vp1{};
//static Viewport* ew1_gctx_vp2{};
static HitTest* ew1_gctx_ht{};

static ProcessWindow* ew1_pw{};
static Input* ew1_pw_input{};

static Shader* vp1_Shader3D_primary{};
static Shader* vp1_Shader2D_primary{};
//static Shader* vp1_Shader2D_font{};

//static Shader* vp2_Shader3D_primary{};
//static Shader* vp2_Shader2D_primary{};

static Camera* vp1_Cam3D_primary{};
static Camera* vp1_Cam2D_primary{};

//static Camera* vp2_Cam3D_primary{};
//static Camera* vp2_Cam2D_primary{};

static Texture* vp1_Tex_fallback{};
static Texture* vp1_Tex_fallback_groundTest{};

static Mesh* vp1_Mesh3D_groundTest{}; //cube{};
//static Mesh* vp1_Mesh3D_pyramid{};
//static Mesh* vp1_Mesh3D_sphere{};
static vector<pair<u8, Mesh*>> vp1_Mesh2D_rects{};

//static Mesh* vp1_Mesh2D_font{};
//static Texture* vp1_Tex_font{};

//static Mesh* vp1_Mesh2D_fontBackground{};
//static Texture* vp1_Tex_fontBackground{};

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

    string err = EngineWindow::GetRegistry().GetContent(0, ew1, false);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get engine window '0'! Reason: " + err);
    }

    err = ProcessWindow::GetRegistry().GetContent(ew1->GetWindowContextID(), ew1_pw);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get process window from engine window '" + to_string(ew1->GetID()) + "'! Reason: " + err);
    }

    ew1_pw->SetMinSize({800, 600});

    err = GraphicsContext::GetRegistry().GetContent(ew1->GetGraphicsContextID(), ew1_gctx);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get graphics context from process window '" + to_string(ew1_pw->GetID()) + "'! Reason: " + err);
    }

    err = HitTest::GetRegistry().GetContent(ew1_gctx->GetHitTestID(), ew1_gctx_ht);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get hit test from graphics context '" + to_string(ew1_gctx->GetID()) + "'! Reason: " + err);
    }

    err = Viewport::GetRegistry().GetContent(ew1_gctx->GetRootViewportID(), ew1_gctx_vp1);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get root viewport from graphics context '" + to_string(ew1_gctx->GetID()) + "'! Reason: " + err);
    }
    ew1_gctx_vp1->SetType(ViewportType::VP_FIT);

    err = Shader::GetRegistry().GetContent(ew1_gctx_vp1->GetShader3DIDs().front(), vp1_Shader3D_primary);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get primary 3D shader from viewport '" + to_string(ew1_gctx_vp1->GetID()) + "'! Reason: " + err);
    }

    err = Shader::GetRegistry().GetContent(ew1_gctx_vp1->GetShader2DIDs().front(), vp1_Shader2D_primary);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get primary 2D shader from viewport '" + to_string(ew1_gctx_vp1->GetID()) + "'! Reason: " + err);
    }

    err = Camera::GetRegistry().GetContent(ew1_gctx_vp1->GetPrimary3DCameraID(), vp1_Cam3D_primary);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get primary 3D camera from viewport '" + to_string(ew1_gctx_vp1->GetID()) + "'! Reason: " + err);
    }
    scast<Transform3D&>(vp1_Cam3D_primary->GetTransform()).setpos({ 0.0f, 1.0f, 0.0f });
    vp1_Cam3D_primary->SetSensitivityMultiplier(0.175f);
    vp1_Cam3D_primary->SetSpeedMultiplier(7.5f);

    err = Camera::GetRegistry().GetContent(ew1_gctx_vp1->GetPrimary2DCameraID(), vp1_Cam2D_primary);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get primary 2D camera from viewport '" + to_string(ew1_gctx_vp1->GetID()) + "'! Reason: " + err);
    }

    err = Input::GetRegistry().GetContent(ew1_pw->GetInputID(), ew1_pw_input);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get input from engine windows process window '" + to_string(ew1_pw->GetID()) + "'! Reason: " + err);
    }

    exePath = KalaWindowCore::GetExePath();

    //Transform3D& c3t = vp1_Cam3D_primary->GetTransform();

    //sync before creating kg objects
    EngineCore::SyncID();

    //
    // CREATE FALLBACK TEXTURE
    //

    vp1_Tex_fallback = Examples::Test_Create_Texture(
        vp1_Shader3D_primary,
        {
            .pixelData = vector<u8>(
                FALLBACK_TEXTURE.begin(), 
                FALLBACK_TEXTURE.end()),
            .filterMode = TextureFilterMode::FILTER_NEAREST,
            .size = 16
        });

    vp1_Tex_fallback_groundTest = Examples::Test_Create_Texture(
        vp1_Shader3D_primary,
        {
            .pixelData = vector<u8>(
                FALLBACK_TEXTURE.begin(), 
                FALLBACK_TEXTURE.end()),
            .filterMode = TextureFilterMode::FILTER_NEAREST,
            .wrapMode = TextureWrapMode::WRAP_REPEAT,
            .size = 16
        });

    //
    // CREATE 3D CUBE
    //

    vp1_Mesh3D_groundTest = Examples::Test_Create_Mesh(
        vp1_Shader3D_primary,
        vp1_Tex_fallback_groundTest,
        Mesh::GenerateMeshData(Mesh_Cube{.edgeCount = 4}));

    vector<Vertex> vertices = vp1_Mesh3D_groundTest->GetVertices();
    vector<u32> indices = vp1_Mesh3D_groundTest->GetIndices();

    vec3 newSize = { 10.0f, epsilon, 10.0f }; 

    for (Vertex& v : vertices)
    {
        v.uv.x *= newSize.x;
        v.uv.y *= newSize.z;
    }

    vp1_Mesh3D_groundTest->SetMeshData(
    {
        .vertices = std::move(vertices),
        .indices = std::move(indices)
    });
    scast<Transform3D&>(vp1_Mesh3D_groundTest->GetTransform()).setsize(newSize);

    /*
    scast<Transform3D&>(vp1_Mesh3D_cube->GetTransform()).setpos(
        c3t.getdirfront() * 2.0f 
        - c3t.getdirright() * 2.0f);
    */

    //
    // CREATE 3D PYRAMID
    //

    /*
    vp1_Mesh3D_pyramid = Examples::Test_Create_Mesh(
        vp1_Shader3D_primary,
        vp1_Tex_fallback,
        Mesh::GenerateMeshData(Mesh_Pyramid{.edgeCount = 4}));

    scast<Transform3D&>(vp1_Mesh3D_pyramid->GetTransform()).setpos(
        c3t.getdirfront() * 2.0f);
    */

    //
    // CREATE 3D SPHERE
    //

    /*
    vp1_Mesh3D_sphere = Examples::Test_Create_Mesh(
        vp1_Shader3D_primary,
        vp1_Tex_fallback,
        Mesh::GenerateMeshData(Mesh_Sphere{}));

    scast<Transform3D&>(vp1_Mesh3D_sphere->GetTransform()).setpos(
        c3t.getdirfront() * 2.0f 
        + c3t.getdirright() * 2.0f);
    */

    //interact with created 3D meshes

    /*
    vp1_Mesh3D_cube->SetKeyHeldCallback(
        KeyboardButton::K_SPACE,
        []() 
        {
            scast<Transform3D&>(vp1_Mesh3D_cube->GetTransform()).addpos({0.0f, 0.05f, 0.0f});
        },
        false);

    vp1_Mesh3D_cube->SetKeyPressedCallback(
        KeyboardButton::K_1,
        []() 
        {
            vp1_Mesh3D_cube->SetTransparentState(!vp1_Mesh3D_cube->IsTransparent());
        },
        false);
    vp1_Mesh3D_cube->SetKeyPressedCallback(
        KeyboardButton::K_2,
        []() 
        {
            static u8 colorIndex{};

            ++colorIndex;
            if (colorIndex == colors.size()) colorIndex = 0;
            vp1_Mesh3D_cube->SetColor(vec4{colors[colorIndex]});
        },
        false);

    vp1_Mesh3D_pyramid->SetKeyPressedCallback(
        KeyboardButton::K_3,
        []() 
        {
            vp1_Mesh3D_pyramid->SetTransparentState(!vp1_Mesh3D_pyramid->IsTransparent());
        },
        false);
    vp1_Mesh3D_pyramid->SetKeyPressedCallback(
        KeyboardButton::K_4,
        []() 
        {
            static u8 colorIndex{};

            ++colorIndex;
            if (colorIndex == colors.size()) colorIndex = 0;
            vp1_Mesh3D_pyramid->SetColor(vec4{colors[colorIndex]});
        },
        false);

    vp1_Mesh3D_sphere->SetKeyPressedCallback(
        KeyboardButton::K_5,
        []() 
        {
            vp1_Mesh3D_sphere->SetTransparentState(!vp1_Mesh3D_sphere->IsTransparent());
        },
        false);
    vp1_Mesh3D_sphere->SetKeyPressedCallback(
        KeyboardButton::K_6,
        []() 
        {
            static u8 colorIndex{};

            ++colorIndex;
            if (colorIndex == colors.size()) colorIndex = 0;
            vp1_Mesh3D_sphere->SetColor(vec4{colors[colorIndex]});
        },
        false);
    */

    //
    // CREATE 2D QUADS
    //

    /*
    vec2 pos{};
    for (int i = 0; i < 5; ++i)
    {
        vp1_Mesh2D_rects.push_back(
        {
            0,
            Examples::Test_Create_Mesh(
            vp1_Shader2D_primary,
            vp1_Tex_fallback,
            {})
        });

        vp1_Mesh2D_rects[i].second->SetViewportAnchorPosition(AnchorPosition::P_TOP_LEFT);
        vp1_Mesh2D_rects[i].second->SetLocalAnchorPosition(AnchorPosition::P_TOP_LEFT);

        Transform2D& t = vp1_Mesh2D_rects[i].second->GetTransform();
        t.setsize(vec2{ 50.0f, 20.0f });
        t.addpos(pos);

        pos.x += 75.0f;
    }
    */

    /*
    //interact with rects

    for (size_t i = 0; i < vp1_Mesh2D_rects.size(); i++)
    {
        Mesh* m = vp1_Mesh2D_rects[i].second;

        m->SetMouseButtonPressedCallback(
            MouseButton::M_LEFT,
            [m, i]()
            {
                ++vp1_Mesh2D_rects[i].first;
                if (vp1_Mesh2D_rects[i].first == colors.size()) vp1_Mesh2D_rects[i].first = 0;
                m->SetColor(vec4{colors[vp1_Mesh2D_rects[i].first]});
                
                Log::Print("@@@@@ pressed lmb over 2D mesh '" + to_string(m->GetID()) + "'..."); 
            });

        m->SetMouseButtonPressedCallback(
            MouseButton::M_RIGHT,
            [m]()
            {
                m->SetTransparentState(!m->IsTransparent());
                
                Log::Print("@@@@@ pressed rmb over 2D mesh '" + to_string(m->GetID()) + "'..."); 
            });

        m->SetOnHoverStartCallback([m]() 
            {
                scast<Transform2D&>(m->GetTransform()).addsize(10);

                Log::Print("@@@@@ started hovering over 2D mesh '" + to_string(m->GetID()) + "'..."); 
            });

        m->SetOnHoverExitCallback([m]() 
            { 
                scast<Transform2D&>(m->GetTransform()).addsize(-10);

                Log::Print("@@@@@ stopped hovering over 2D mesh '" + to_string(m->GetID()) + "'..."); 
            });
    }
    */

    //
    // CREATE FONT MESH
    //

    /*
    string _ = Shader::GetRegistry().GetContent(
        ew1_gctx_vp1->GetRootShaderID(RootShaderTarget::T_FONT),
        vp1_Shader2D_font);

    vp1_Tex_font = Texture::Initialize(
        vp1_Shader2D_font->GetID(),
        { 
            .format = TexturePixelFormat::FORMAT_BASIC_R8,
            //.filterMode = TextureFilterMode::FILTER_NEAREST
        });

    vp1_Mesh2D_font = Examples::Test_Create_Mesh(
        vp1_Shader2D_font,
        vp1_Tex_font,
        {});

    //font color is black
    vp1_Mesh2D_font->SetColor( { vec3{ 0.0f }, 1.0f } );

    scast<Transform2D&>(vp1_Mesh2D_font->GetTransform()).setsize(50);
    vp1_Mesh2D_font->SetViewportAnchorPosition(AnchorPosition::P_TOP_RIGHT);
    vp1_Mesh2D_font->SetLocalAnchorPosition(AnchorPosition::P_TOP_RIGHT);

    vp1_Tex_fontBackground = Texture::Initialize(
        vp1_Shader2D_primary->GetID(),
        {});

    vp1_Mesh2D_fontBackground = Examples::Test_Create_Mesh(
        vp1_Shader2D_primary,
        vp1_Tex_fontBackground,
        {});

    scast<Transform2D&>(vp1_Mesh2D_fontBackground->GetTransform()).setsize(50);
    vp1_Mesh2D_fontBackground->SetViewportAnchorPosition(AnchorPosition::P_TOP_RIGHT);
    vp1_Mesh2D_fontBackground->SetLocalAnchorPosition(AnchorPosition::P_TOP_RIGHT);

    vp1_Mesh2D_font->SetDrawOrderIndex(100);
    vp1_Mesh2D_fontBackground->SetDrawOrderIndex(50);
    */

    //
    // SELECT AND INITIALIZE FONT
    //

    /*
    //initialize from selected font path

    vector<path> files = Window_Global::GetFiles(
        FileType::FILE_CUSTOM,
        {
            ".ttf",
            ".otf"
            },
            path(exePath.parent_path() / "files" / "fonts"));

    if (files.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to import font because no font was selected!");
    }

    string fontName = files.front().stem().string();
    font = ImportFont::Initialize(
        path(files.front()),
        64);
    */

    /*
    //initialize from known font path

    path fontName = path("LeagueGothic") / "LeagueGothic-Regular.otf";
    path fontPath = exePath.parent_path() / "files" / "fonts" / fontName;

    font = ImportFont::Initialize(
        path(fontPath),
        64);

    if (!font)
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to import font '" + fontName.string() + "'!");
    }
    */

    /*
    Examples::Test_Print_Glyph_Atlas_To_Texture(
        font,
        vp1_Tex_font,
        vp1_Mesh2D_font,
        vp1_Mesh2D_fontBackground);
    */

    //
    // CREATE SECOND VIEWPORT
    //

    /*
    ew1_gctx_vp2 = Viewport::Initialize(ew1_gctx->GetID());

    _ = Shader::GetRegistry().GetContent(
        ew1_gctx_vp2->GetRootShaderID(RootShaderTarget::T_UNLIT),
        vp2_Shader3D_primary);

    _ = Shader::GetRegistry().GetContent(
        ew1_gctx_vp2->GetRootShaderID(RootShaderTarget::T_RECT),
        vp2_Shader2D_primary);
        
    _ = Camera::GetRegistry().GetContent(
        ew1_gctx_vp2->GetPrimary3DCameraID(),
        vp2_Cam3D_primary);
        
    _ = Camera::GetRegistry().GetContent(
        ew1_gctx_vp2->GetPrimary2DCameraID(),
        vp2_Cam2D_primary);

    ew1_gctx_vp2->SetDynamicResizeState(false);
    ew1_gctx_vp2->SetType(ViewportType::VP_FILL);
    ew1_gctx_vp2->SetSize(250);
    ew1_gctx_vp2->SetOffset(0);
    ew1_gctx_vp2->SetBackgroundColor(1);
    ew1_gctx_vp2->SetVisibleState(false);
    */

    //sync after kg objects are done with initialization
    EngineCore::SyncID();

    //Examples::Test_Popup_And_File_Drag(ew1_pw);
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
            fps,
            "GAME_CORE",
            LogType::LOG_INFO);
    }

    Examples::Test_VSync_Input(
        ew1_gctx,
        ew1_pw_input);

    Examples::Test_Camera_Toggle(ew1_pw_input);

    Examples::Test_Camera_Move(
        ew1_pw_input,
        vp1_Cam3D_primary,
        EngineCore::GetDeltaTime());

    /*
    Examples::Test_Create_Notification(ew1_pw_input);

    Examples::Test_System_Sound_Input(ew1_pw_input);

    Examples::Test_Get_Files(
        ew1_pw_input,
        { 
            "*.spv",
            "*.vert",
            "*.frag",
            "*.txt",
            "*.md" },
        path{exePath}.parent_path());

    Examples::Test_Window_Toggles(
        ew1_pw,
        ew1_pw_input);

    Examples::Test_Texture_Filter_Mode(
        ew1_pw_input,
        vp1_Tex_fallback);

    Examples::Test_Mesh_Toggle_Recreate_Target(ew1_pw_input);
    Examples::Test_Mesh_Recreate_Cube_On_Mouse_Actions(
        ew1_pw_input,
        vp1_Mesh3D_cube);
    Examples::Test_Mesh_Recreate_Pyramid_On_Mouse_Actions(
        ew1_pw_input,
        vp1_Mesh3D_pyramid);
    Examples::Test_Mesh_Recreate_Sphere_On_Mouse_Actions(
        ew1_pw_input,
        vp1_Mesh3D_sphere);
    */

    /*
    Examples::Test_Print_Glyph_To_Console(
        ew1_pw_input,
        impf);

    Examples::Test_Toggle_From_Atlas_State(ew1_pw_input);

    Examples::Test_Print_Glyph_To_Texture(
        ew1_pw_input,
        font,
        vp1_Tex_font,
        vp1_Mesh2D_font,
        vp1_Mesh2D_fontBackground);
    */
}

void ElypsoEngine::Core::LateUpdate()
{
    
}