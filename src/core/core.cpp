//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>

#include "log_utils.hpp"
#include "string_utils.hpp"
#include "key_standards.hpp"

#include "test/test_examples.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_window.hpp"
#include "graphics/ee_scene.hpp"
#include "graphics/kw_window.hpp"
#include "graphics/kw_window_global.hpp"
#include "core/kw_input.hpp"
#include "core/kw_core.hpp"
#include "graphics/kg_context.hpp"
#include "graphics/kg_viewport.hpp"
#include "graphics/kg_hit_test.hpp"
#include "graphics/kg_shader.hpp"
#include "graphics/kg_mesh.hpp"
#include "graphics/kg_material.hpp"
#include "graphics/kg_texture.hpp"
#include "graphics/kg_camera.hpp"
#include "graphics/kg_material.hpp"
#include "import/kg_import_font.hpp"
#include "widgets_primitive/kg_widget_text.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaMath::epsilon;
using KalaHeaders::KalaMath::vec4;
using KalaHeaders::KalaMath::vec3;
using KalaHeaders::KalaMath::Transform3D;
using KalaHeaders::KalaMath::Transform2D;

using KalaHeaders::KalaKeyStandards::KeyboardButton;

using MetalMetropolis::Test::Examples;

using ElypsoEngine::Core::AppConfig;
using ElypsoEngine::Graphics::EngineWindow;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::FileType;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Core::Input;
using KalaWindow::Core::KalaWindowCore;
using KalaGraphics::Graphics::GraphicsContext;
using KalaGraphics::Graphics::RootShaderTarget;
using KalaGraphics::Graphics::Viewport;
using KalaGraphics::Graphics::ViewportType;
using KalaGraphics::Graphics::Shader;
using KalaGraphics::Graphics::HitTest;
using KalaGraphics::Graphics::AnchorPosition;
using KalaGraphics::Graphics::Mesh_Cube;
using KalaGraphics::Graphics::Mesh;
using KalaGraphics::Graphics::Vertex;
using KalaGraphics::Graphics::MaterialType2D;
using KalaGraphics::Graphics::Material;
using KalaGraphics::Graphics::Texture;
using KalaGraphics::Graphics::TexturePixelFormat;
using KalaGraphics::Graphics::TextureFilterMode;
using KalaGraphics::Graphics::TextureWrapMode;
using KalaGraphics::Graphics::Camera;
using KalaGraphics::Import::ImportFont;
using KalaGraphics::PrimitiveWidgets::Text;

using std::string;
using std::filesystem::path;
using std::vector;

static path exePath{};

static ImportFont* font{};

static EngineWindow* ew1{};
static GraphicsContext* ew1_gctx{};
static Viewport* ew1_gctx_vp1{};
static HitTest* ew1_gctx_ht{};

static ProcessWindow* ew1_pw{};
static Input* ew1_pw_input{};

static Shader* vp1_Shader3D_primary{};
static Shader* vp1_Shader2D_primary{};
static Shader* vp1_Shader2D_font{};

static Camera* vp1_Cam3D_primary{};
static Camera* vp1_Cam2D_primary{};

static Texture* vp1_Tex_root{};
static Texture* vp1_Tex_fallback{};
static Texture* vp1_Tex_fallback_groundTest{};

static vector<Mesh*> vp1_Mesh3D_importedMeshes{};
static Mesh* vp1_Mesh3D_groundTest{};

static Mesh* vp1_Mesh2D_font{};
static Texture* vp1_Tex_font{};

static Mesh* vp1_Mesh2D_fontBackground{};
static Texture* vp1_Tex_fontBackground{};

static Text* vp1_Text_typeTest{};
static Mesh* vp1_Mesh2D_typeTest{};

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

    err = Texture::GetRegistry().GetContent(vp1_Shader3D_primary->GetRootTextureID(), vp1_Tex_root);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get root texture from shader '" + to_string(vp1_Shader3D_primary->GetID()) + "'! Reason: " + err);
    }

    err = Texture::GetRegistry().GetContent(vp1_Shader3D_primary->GetFallbackTextureID(), vp1_Tex_fallback);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get fallback texture from shader '" + to_string(vp1_Shader3D_primary->GetID()) + "'! Reason: " + err);
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
    // CREATE TEXTURES
    //

    Texture* fallbackTex{};
    err = Texture::GetRegistry().GetContent(vp1_Shader3D_primary->GetFallbackTextureID(), fallbackTex);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get fallback texture from shader '" + to_string(vp1_Shader3D_primary->GetID()) + "'! Reason: " + err);
    }

    vp1_Tex_fallback_groundTest = Examples::Test_Create_Texture(
        vp1_Shader3D_primary,
        {
            .pixelData = vector<u8>(fallbackTex->GetPixelData()),
            .filterMode = TextureFilterMode::FILTER_NEAREST,
            .wrapMode = TextureWrapMode::WRAP_REPEAT,
            .size = 16
        });

    //
    // CREATE GROUND
    //

    vp1_Mesh3D_groundTest = Examples::Test_Create_Mesh(
        vp1_Shader3D_primary,
        Mesh::GenerateMeshData(Mesh_Cube{.edgeCount = 4}));

    Material* groundMeshMat{};
    err = Material::GetRegistry().GetContent(vp1_Mesh3D_groundTest->GetMaterialID(), groundMeshMat);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to initialize ground mesh because its material was invalid! Reason: " + err);
    }

    groundMeshMat->SetBaseColorTextureID(vp1_Tex_fallback_groundTest->GetID());

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

    //
    // SELECT AND INITIALIZE FONT
    //

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

    //initialize from known font path

    /*
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
    // CREATE TEST FONT
    //

    vp1_Text_typeTest = Text::Initialize(
        font->GetID(),
        ew1_gctx_vp1->GetID());

    vp1_Text_typeTest->SetEditState(true);

    Mesh* vp1_Mesh2D_typeTest{};
    err = Mesh::GetRegistry().GetContent(vp1_Text_typeTest->GetMeshID(), vp1_Mesh2D_typeTest);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to initialize text because its mesh '" 
            + to_string(vp1_Text_typeTest->GetMeshID()) + "' was invalid! Reason: " + err);
    }

    vp1_Mesh2D_typeTest->SetViewportAnchorPosition(AnchorPosition::P_CENTER);
    vp1_Mesh2D_typeTest->SetLocalAnchorPosition(AnchorPosition::P_CENTER);
    scast<Transform2D&>(vp1_Mesh2D_typeTest->GetTransform()).addpos({ 0.0f, 50.0f });

    //sync after kg objects are done with initialization
    EngineCore::SyncID();
}

void ElypsoEngine::Core::EarlyUpdate()
{

}

void ElypsoEngine::Core::FixedUpdate()
{

}

void ElypsoEngine::Core::Update()
{
    Examples::Test_Camera_Toggle(ew1_pw_input);

    Examples::Test_Camera_Move(
        ew1_pw_input,
        vp1_Cam3D_primary,
        EngineCore::GetDeltaTime());

    /*
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