//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "log_utils.hpp"
#include "math_utils.hpp"
#include "key_standards.hpp"

#include "core/ee_core.hpp"
#include "graphics/ee_window.hpp"
#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"
#include "graphics/kw_window_global.hpp"
#include "graphics/kg_context.hpp"
#include "graphics/kg_hit_test.hpp"
#include "graphics/kg_viewport.hpp"
#include "graphics/kg_shader.hpp"
#include "graphics/kg_camera.hpp"
#include "graphics/kg_material.hpp"
#include "graphics/kg_texture.hpp"
#include "graphics/kg_mesh.hpp"
#include "widgets/kg_text.hpp"
#include "import/kg_import_font.hpp"
#include "import/kg_import_mesh.hpp"

#include "render.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaMath::epsilon;
using KalaHeaders::KalaMath::vec2;
using KalaHeaders::KalaMath::vec3;
using KalaHeaders::KalaMath::vec4;
using KalaHeaders::KalaMath::Transform2D;
using KalaHeaders::KalaMath::Transform3D;
using KalaHeaders::KalaMath::PosTarget;
using KalaHeaders::KalaMath::RotTarget;
using KalaHeaders::KalaMath::SizeTarget;

using KalaHeaders::KalaKeyStandards::KeyboardButton;

using ElypsoEngine::Core::EngineCore;
using ElypsoEngine::Graphics::EngineWindow;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::Input;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::FileType;
using KalaWindow::Graphics::Window_Global;
using KalaGraphics::Graphics::VSyncState;
using KalaGraphics::Graphics::GraphicsContext;
using KalaGraphics::Graphics::HitTest;
using KalaGraphics::Graphics::ViewportType;
using KalaGraphics::Graphics::Viewport;
using KalaGraphics::Graphics::Shader;
using KalaGraphics::Graphics::Camera;
using KalaGraphics::Graphics::AlphaMode;
using KalaGraphics::Graphics::Material;
using KalaGraphics::Graphics::TextureFilterMode;
using KalaGraphics::Graphics::TextureWrapMode;
using KalaGraphics::Graphics::TextureData;
using KalaGraphics::Graphics::Texture;
using KalaGraphics::Graphics::Vertex;
using KalaGraphics::Graphics::AnchorPosition;
using KalaGraphics::Graphics::Mesh_Cube;
using KalaGraphics::Graphics::MeshData;
using KalaGraphics::Graphics::Mesh;
using KalaGraphics::Widgets::Text;
using KalaGraphics::Import::ImportFont;
using KalaGraphics::Import::ImportNodeData;
using KalaGraphics::Import::ImportPrimitiveData;
using KalaGraphics::Import::ImportMaterialData;
using KalaGraphics::Import::ImportMesh;

using std::string;
using std::to_string;
using std::format;
using std::array;
using std::vector;
using std::filesystem::path;
using std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration;

static bool isInitialized{};

static bool isCameraMovable{};

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

static Camera* vp1_Cam3D_primary{};
static Camera* vp1_Cam2D_primary{};

static Texture* vp1_Tex_root{};
static Texture* vp1_Tex_fallback{};
static Texture* vp1_Tex_fallback_groundTest{};

static vector<Mesh*> vp1_Mesh3D_importedMeshes{};
static Mesh* vp1_Mesh3D_groundTest{};

static Text* vp1_Text_typeTest{};
static Mesh* vp1_Mesh2D_typeTest{};

static Mesh* vp1_Mesh2D_typeTestBackground{};

static array<string, 4> GetFPS(f64 secondsToWait)
{
    static f64 cachedFPS = EngineCore::GetAverageFPS();
    static f64 cachedFrameLength = EngineCore::GetAverageFrameLength();
    static f64 cachedOnePercentFPS = EngineCore::GetOnePercentLowFPS();
    static f64 cachedZeroOnePercentFPS = EngineCore::GetZeroPointOnePercentLowFPS();

    static time_point start = steady_clock::now();

    if (steady_clock::now() - start > duration<f64>(secondsToWait))
    {
        cachedFPS = EngineCore::GetAverageFPS();
        cachedFrameLength = EngineCore::GetAverageFrameLength();
        cachedOnePercentFPS = EngineCore::GetOnePercentLowFPS();
        cachedZeroOnePercentFPS = EngineCore::GetZeroPointOnePercentLowFPS();

        start = steady_clock::now();
    }
    
    return
    {
        format("average: {:.2f} fps", cachedFPS),
        format("frame length: {:.2f}ms", cachedFrameLength),
        format("1% low: {:.2f} fps", cachedOnePercentFPS),
        format("0.1% low: {:.2f} fps", cachedZeroOnePercentFPS)
    };
}

static void ToggleVSync(
    GraphicsContext* gctx,
    Input* input)
{
    //if (input->IsComboPressed(combo_vsync_disable))
    if (input->IsKeyPressed(KeyboardButton::K_1))
    {
        Log::Print("@@@@@ set vsync state to off");
        gctx->SetVSyncState(VSyncState::VSYNC_OFF);
    }
    //else if (input->IsComboPressed(combo_vsync_adaptive))
    else if (input->IsKeyPressed(KeyboardButton::K_2))
    {
        Log::Print("@@@@@ set vsync state to adaptive");
        gctx->SetVSyncState(VSyncState::VSYNC_ON_ADAPTIVE);
    }
    //else if (input->IsComboPressed(combo_vsync_triple_buffered))
    if (input->IsKeyPressed(KeyboardButton::K_3))
    {
        Log::Print("@@@@@ set vsync state to triple buffered");
        gctx->SetVSyncState(VSyncState::VSYNC_ON_TRIPLE_BUFFERED);
    }
}

static void ToggleCamera(Input* input)
{
    //if (input->IsComboPressed(combo_toggle_cam_move))
    if (input->IsKeyPressed(KeyboardButton::K_ESC))
    {
        isCameraMovable = !isCameraMovable;

        if (!isCameraMovable)
        {
            input->SetMouseLockState(false);
            input->SetMouseVisibility(true);
        }
        else
        {
            input->SetMouseLockState(true);
            input->SetMouseVisibility(false);
        }

        Log::Print("@@@@@ set cam move state to: " 
            + string(isCameraMovable ? "on" : "off"));
    }
}

static void MoveCamera(
    Input* input,
    Camera* cam,
    f32 deltaTime)
{
    static f32 sprintMultiplier = 2.0f;

    if (!isCameraMovable) return;

    vec2 mouse = input->GetRawMouseDelta();
    vec2 kb{};
    f32 vertical{};

    //forwards
    if (input->IsKeyHeld(KeyboardButton::K_W)) kb.y += 1.0f;
    //backwards
    if (input->IsKeyHeld(KeyboardButton::K_S)) kb.y -= 1.0f;
    //left
    if (input->IsKeyHeld(KeyboardButton::K_A)) kb.x -= 1.0f;
    //right
    if (input->IsKeyHeld(KeyboardButton::K_D)) kb.x += 1.0f;

    //up
    if (input->IsKeyHeld(KeyboardButton::K_E)) vertical += 1.0f;
    //down
    if (input->IsKeyHeld(KeyboardButton::K_Q)) vertical -= 1.0f;

    //sprint
    if (input->IsKeyHeld(KeyboardButton::K_LEFT_SHIFT)) deltaTime *= sprintMultiplier;

    /*
    string moveStr = "\nmouse - x: " 
        + to_string(mouse.x) + ", y: "
        + to_string(mouse.y) + "\nkb - x: "
        + to_string(kb.x) + ", y: "
        + to_string(kb.y);

    Transform3D& ct = cam->GetTransform();
    vec3 euler = toeuler3(ct.rot_world);

    string transformStr = "\npos - x: "
        + to_string(ct.pos_world.x) + ", y: "
        + to_string(ct.pos_world.y) + ", z: "
        + to_string(ct.pos_world.z) + ", \nrot - x: "
        + to_string(euler.x) + ", y: "
        + to_string(euler.y) + ", z: "
        + to_string(euler.z) + ", \nsize - x: "
        + to_string(ct.size_world.x) + ", y: "
        + to_string(ct.size_world.y) + ", z: "
        + to_string(ct.size_world.z);

    Log::Print(
        moveStr + transformStr,
        "GAME_CAM_MOVE_TEST",
        LogType::LOG_VERBOSE);
    */

    cam->Move(
        mouse,
        kb,
        vertical,
        deltaTime);
}

static void ImportMeshes(
    Input* input,
    Shader* shader)
{
    static vector<Mesh*> importedMeshes{};

    if (input->IsKeyPressed(KeyboardButton::K_SPACE))
    {
        vector<path> files = Window_Global::GetFiles(
            FileType::FILE_CUSTOM,
            {
                ".glb",
                ".gltf"
                });

        if (files.empty())
        {
            Log::Print(
                "Failed to import glb/gltf file because none was selected!",
                "GAME_CORE",
                LogType::LOG_WARNING);

            return;
        }
        else
        {
            ImportMesh* importMesh = ImportMesh::Initialize(path(files.front()));
            if (!importMesh)
            {
                Log::Print(
                    "Failed to import mesh from path '" + files.front().string() + "'!",
                    "GAME_CORE",
                    LogType::LOG_ERROR,
                    2);

                return;
            }
            else
            {
                vector<u32> meshIDs{};

                if (!importedMeshes.empty())
                {
                    for (Mesh* m : importedMeshes) m->Destroy();
                    importedMeshes.clear();
                }

                for (const ImportNodeData& nodeData : importMesh->GetMeshData())
                {
                    for (const ImportPrimitiveData& primitiveData : nodeData.primitiveData)
                    {
                        Mesh* primitive = Mesh::Initialize(shader->GetID());

                        if (!primitive)
                        {
                            KalaWindowCore::ForceClose(
                                "Metal Metropolis core error",
                                "Failed to import model '" + files.front().string() 
                                + "' because one of its primitives failed to initialize!");
                        }

                        Material* mat{};
                        string err = Material::GetRegistry().GetContent(primitive->GetMaterialID(), mat);
                        if (!err.empty())
                        {
                            KalaWindowCore::ForceClose(
                                "Game core error",
                                "Failed to initialize mesh because its material was invalid! Reason: " + err);
                        }

                        primitive->SetMeshData(
                        {
                            .vertices = vector<Vertex>(primitiveData.meshData.vertices),
                            .indices = vector<u32>(primitiveData.meshData.indices)
                        });

                        Transform3D& mt = primitive->GetTransform();
                        mt.setpos(nodeData.transform.getpos(PosTarget::POS_LOCAL));
                        mt.setrotquat(nodeData.transform.getrotquat(RotTarget::ROT_LOCAL));
                        mt.setsize(nodeData.transform.getsize(SizeTarget::SIZE_LOCAL));
                        mt.update();

                        primitive->FlipFaceDirection();

                        const ImportMaterialData& matData = primitiveData.matData;

                        mat->SetBaseColor(vec4(matData.baseColor));

                        if (matData.alphaMode != AlphaMode::A_OPAQUE)
                        {
                            mat->SetAlphaMode(matData.alphaMode);

                            if (matData.alphaMode == AlphaMode::A_MASK)
                            {
                                mat->SetAlphaCutoff(matData.alphaCutoff);
                            }
                        }

                        if (!matData.textureData.pixelData.empty())
                        {
                            Texture* meshTex = Texture::Initialize(shader->GetID());

                            meshTex->SetPixelData(vector<u8>(matData.textureData.pixelData));
                            meshTex->SetSize(matData.textureData.size);
                            meshTex->SetPixelFormat(matData.textureData.pixelFormat);

                            mat->SetBaseColorTextureID(meshTex->GetID());
                        }

                        importedMeshes.push_back(primitive);

                        meshIDs.push_back(primitive->GetID());
                    }
                }

                /*
                Export::ExportMeshes(meshIDs, path(
                    files.front().parent_path() 
                    / (files.front().stem().string() + "_1.glb")));

                Log::Print(
                    "@@@@@\n"
                    "json data:\n" + Export::GetMeshJsonData(meshIDs));
                */
            }
        }
    }
}

static Texture* CreateTexture(
    Shader* shader,
    TextureData&& textureData)
{
    //sync ids before generating texture
    EngineCore::SyncID();

    Texture* texture = Texture::Initialize(
        shader->GetID(),
        std::move(textureData));
    if (!texture)
    {
        KalaWindowCore::ForceClose(
            "Game core error",
            "Failed to initialize test texture!");
    }

    return texture;
}

static Mesh* CreateMesh(
        Shader* shader,
        MeshData&& meshData)
{
    //sync ids before generating mesh
    EngineCore::SyncID();

    Mesh* mesh = Mesh::Initialize(shader->GetID());
    if (!mesh)
    {
        KalaWindowCore::ForceClose(
            "Game core error",
            "Failed to initialize test mesh!");
    }

    Material* mat{};
    string err = Material::GetRegistry().GetContent(mesh->GetMaterialID(), mat);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Game core error",
            "Failed to initialize test mesh because its material was invalid! Reason: " + err);
    }

    if (!mesh->Is2D()) mesh->SetMeshData(std::move(meshData));

    return mesh;
}

static void GetCoreStuff()
{
    exePath = KalaWindowCore::GetExePath();

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
    Transform3D& camTransform = scast<Transform3D&>(vp1_Cam3D_primary->GetTransform());
    camTransform.setpos({ 0.0f, 1.0f, 0.0f });
    camTransform.update();
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
}

static void InitializeGameStuff()
{
    //sync before creating kg objects
    EngineCore::SyncID();

    //
    // CREATE TEXTURES
    //

    Texture* fallbackTex{};
    string err = Texture::GetRegistry().GetContent(vp1_Shader3D_primary->GetFallbackTextureID(), fallbackTex);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to get fallback texture from shader '" + to_string(vp1_Shader3D_primary->GetID()) + "'! Reason: " + err);
    }

    vp1_Tex_fallback_groundTest = CreateTexture(
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

    vp1_Mesh3D_groundTest = CreateMesh(
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
    Transform3D& groundTransform = scast<Transform3D&>(vp1_Mesh3D_groundTest->GetTransform());
    groundTransform.setsize(newSize);
    groundTransform.update();

    //
    // SELECT AND INITIALIZE FONT
    //

    //initialize from selected font path

    /*
    vector<path> files = Window_Global::GetFiles(
        FileType::FILE_CUSTOM,
        {
            ".ttf",
            ".otf"
            });

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

    //initialize from known font path

    path fontName = path("LeagueGothic") / "LeagueGothic-Regular.otf";
    path fontPath = exePath.parent_path() / "files" / "fonts" / fontName;

    font = ImportFont::Initialize(
        path(fontPath),
        32);

    if (!font)
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to import font '" + fontName.string() + "'!");
    }

    /*
    Test_Print_Glyph_Atlas_To_Texture(
        font,
        vp1_Tex_font,
        vp1_Mesh2D_font,
        vp1_Mesh2D_fontBackground);
    */

    //
    // CREATE TEXT WIDGET
    //

    vp1_Text_typeTest = Text::Initialize(
        font->GetID(),
        ew1_gctx_vp1->GetID());
    
    //vp1_Text_typeTest->SetEditState(true);
    vp1_Text_typeTest->SetMaxCharacters(50);

    err = Mesh::GetRegistry().GetContent(vp1_Text_typeTest->GetMeshID(), vp1_Mesh2D_typeTest);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to initialize text because its mesh '" 
            + to_string(vp1_Text_typeTest->GetMeshID()) + "' was invalid! Reason: " + err);
    }

    vp1_Mesh2D_typeTest->SetIgnoreHoverState(false);

    vp1_Mesh2D_typeTest->SetViewportAnchorPosition(AnchorPosition::P_TOP_LEFT);
    vp1_Mesh2D_typeTest->SetSelfAnchorPosition(AnchorPosition::P_TOP_LEFT);

    Material* typeTestMeshMat{};
    err = Material::GetRegistry().GetContent(vp1_Mesh2D_typeTest->GetMaterialID(), typeTestMeshMat);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to initialize text because its mesh material '" 
            + to_string(vp1_Mesh2D_typeTest->GetMaterialID()) + "' was invalid! Reason: " + err);
    }
    typeTestMeshMat->SetBaseColor( vec4{0.0f, 0.0f, 0.0f, 1.0f} );
    typeTestMeshMat->SetAlphaMode(AlphaMode::A_BLEND);

    vp1_Mesh2D_typeTestBackground = Mesh::Initialize(vp1_Shader2D_primary->GetID());

    vp1_Mesh2D_typeTestBackground->SetViewportAnchorPosition(AnchorPosition::P_TOP_LEFT);
    vp1_Mesh2D_typeTestBackground->SetSelfAnchorPosition(AnchorPosition::P_TOP_LEFT);

    Material* typeTestBackgroundMat{};
    err = Material::GetRegistry().GetContent(vp1_Mesh2D_typeTestBackground->GetMaterialID(), typeTestBackgroundMat);
    if (!err.empty())
    {
        KalaWindowCore::ForceClose(
            "Metal Metropolis core error",
            "Failed to initialize text because its background mesh material '" 
            + to_string(vp1_Mesh2D_typeTestBackground->GetMaterialID()) + "' was invalid! Reason: " + err);
    }
    typeTestBackgroundMat->SetBaseColor( vec4{0.5f, 0.5f, 0.8f, 0.3f} );
    typeTestBackgroundMat->SetAlphaMode(AlphaMode::A_BLEND);

    //sync after kg objects are done with initialization
    EngineCore::SyncID();
}

namespace MetalMetropolis::Core
{
    bool Render::IsInitialized() { return isInitialized; }
    void Render::Initialize()
    {
        if (isInitialized)
        {
            Log::Print(
                "Failed to initialize renderer because it has already been initialized!",
                "MM_RENDER",
                LogType::LOG_WARNING);

            return;
        }

        GetCoreStuff();
        InitializeGameStuff();

        isInitialized = true;
    }

    void Render::Update()
    {
        ToggleCamera(ew1_pw_input);

        MoveCamera(
            ew1_pw_input,
            vp1_Cam3D_primary,
            EngineCore::GetDeltaTime());

        ImportMeshes(ew1_pw_input, vp1_Shader3D_primary);

        ToggleVSync(ew1_gctx, ew1_pw_input);

        static string lastFPS{};
        string fps = GetFPS(0.5f)[0];

        if (lastFPS != ""
            && fps == "")
        {
            lastFPS = fps;
            vp1_Text_typeTest->SetText("");
        }
        if (lastFPS != fps)
        {
            lastFPS = fps;
            vp1_Text_typeTest->SetText(fps, true);
        }

        Transform2D& typeTestTransform = scast<Transform2D&>(vp1_Mesh2D_typeTest->GetTransform());
        Transform2D& typeTestBackgroundTransform = scast<Transform2D&>(vp1_Mesh2D_typeTestBackground->GetTransform());

        const vec2 textSize = typeTestTransform.getsize(SizeTarget::SIZE_LOCAL);
        const vec2 textBackgroundSize = typeTestBackgroundTransform.getsize(SizeTarget::SIZE_LOCAL);

        if (textBackgroundSize != textSize)
        {
            typeTestBackgroundTransform.setsize(textSize);
            typeTestBackgroundTransform.update();
        }

        /*
        Test_Print_Glyph_To_Texture(
            ew1_pw_input,
            font,
            vp1_Tex_font,
            vp1_Mesh2D_font,
            vp1_Mesh2D_fontBackground);
        */
    }
}