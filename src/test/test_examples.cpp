//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <format>
#include <chrono>
#include <vector>
#include <filesystem>

#include "log_utils.hpp"
#include "math_utils.hpp"
#include "string_utils.hpp"
#include "key_standards.hpp"

#include "test/test_examples.hpp"
#include "core/ee_core.hpp"
#include "core/kw_core.hpp"
#include "graphics/kw_window_global.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaMath::vec2;
using KalaHeaders::KalaMath::vec3;
using KalaHeaders::KalaMath::Transform2D;

using KalaHeaders::KalaString::IsAlpha;
using KalaHeaders::KalaString::IsNumber;

using KalaHeaders::KalaKeyStandards::KeyboardButton;
using KalaHeaders::KalaKeyStandards::MouseButton;
using KalaHeaders::KalaKeyStandards::GetUTFByKey;
using KalaHeaders::KalaKeyStandards::GetValueByUTF;

using ElypsoEngine::Core::EngineCore;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::SoundType;
using KalaWindow::Graphics::FileType;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupType;
using KalaWindow::Graphics::WindowMode;
using KalaWindow::Core::InputCode;
using KalaGraphics::Core::VSyncState;
using KalaGraphics::Resources::FaceDirection;
using KalaGraphics::Resources::NormalType;
using KalaGraphics::Resources::Mesh_Cube;
using KalaGraphics::Resources::Mesh_Pyramid;
using KalaGraphics::Resources::Mesh_Sphere;
using KalaGraphics::Resources::TextureFilterMode;
using KalaGraphics::Import::FontData;
using KalaGraphics::Import::GlyphData;

using std::string;
using std::to_string;
using std::format;
using std::chrono::time_point;
using std::chrono::steady_clock;
using std::chrono::seconds;
using std::chrono::duration;
using std::format;
using std::vector;
using std::filesystem::path;

static bool fpsState{};
static bool isCameraMovable{};

static bool fromAtlasState{};

static constexpr InputCode combo_vsync_disable[] =
{
    { .kb = KeyboardButton::K_Z },
    { .kb = KeyboardButton::K_1 }
};
static constexpr InputCode combo_vsync_adaptive[] =
{
    { .kb = KeyboardButton::K_Z },
    { .kb = KeyboardButton::K_2 }
};
static constexpr InputCode combo_vsync_triple_buffered[] =
{
    { .kb = KeyboardButton::K_Z },
    { .kb = KeyboardButton::K_3 }
};
static constexpr InputCode combo_toggle_fps[] =
{
    { .kb = KeyboardButton::K_Z },
    { .kb = KeyboardButton::K_4 }
};

static constexpr InputCode combo_sound_ok[] =
{
    { .kb = KeyboardButton::K_X },
    { .kb = KeyboardButton::K_1 }
};
static constexpr InputCode combo_sound_error[] =
{
    { .kb = KeyboardButton::K_X },
    { .kb = KeyboardButton::K_2 }
};

static constexpr InputCode combo_notification[] =
{
    { .kb = KeyboardButton::K_C },
    { .kb = KeyboardButton::K_1 }
};

static constexpr InputCode combo_files_any_one[] =
{
    { .kb = KeyboardButton::K_V },
    { .kb = KeyboardButton::K_1 }
};
static constexpr InputCode combo_files_dir_mult[] =
{
    { .kb = KeyboardButton::K_V },
    { .kb = KeyboardButton::K_2 }
};
static constexpr InputCode combo_files_selected_mult[] =
{
    { .kb = KeyboardButton::K_V },
    { .kb = KeyboardButton::K_3 }
};

static constexpr InputCode combo_toggle_full_screen[] =
{
    { .kb = KeyboardButton::K_B },
    { .kb = KeyboardButton::K_1 }
};
static constexpr InputCode combo_toggle_resizable[] =
{
    { .kb = KeyboardButton::K_B },
    { .kb = KeyboardButton::K_2 }
};

static constexpr InputCode combo_toggle_texture_filter_mode[] =
{
    { .kb = KeyboardButton::K_N },
    { .kb = KeyboardButton::K_1 }
};

static constexpr InputCode combo_toggle_cam_move[] =
{
    { .kb = KeyboardButton::K_M },
    { .kb = KeyboardButton::K_1 }
};

namespace MetalMetropolis::Test
{
    string Examples::Test_Get_FPS(f64 secondsToWait)
    {
        if (!fpsState) return "";

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
        
        return format(
            "\nAverage: {:.2f} fps,"
            "\nframe length: {:.2f}ms,"
            "\n1% low: {:.2f} fps,"
            "\n0.1% low: {:.2f} fps",
            cachedFPS,
            cachedFrameLength,
            cachedOnePercentFPS,
            cachedZeroOnePercentFPS);
    }

    void Examples::Test_Popup_And_File_Drag(ProcessWindow* pw)
    {
        pw->SetDraggedFilesCallback([](const vector<path>& files, vec2 pos)
            {
                if (files.empty())
                {
                    (void)Window_Global::CreatePopup(
                        "File drag result popup",
                        "This popup confirms that no files were dragged on screen!",
                        PopupAction::POPUP_ACTION_OK,
                        PopupType::POPUP_TYPE_WARNING);
                }
                else
                {
                    string filesString{};
                    for (const path& f : files)
                    {
                        filesString += f.string() + ", ";
                    }
                    filesString.pop_back();
                    filesString.pop_back();

                    string posString = 
                        to_string((i32)pos.x) + ", " 
                        + to_string((i32)pos.y);

                    (void)Window_Global::CreatePopup(
                        "File drag result popup",
                        "This popup confirms that files '" + filesString 
                        + "' were dragged on screen at pos '" + posString + "'!",
                        PopupAction::POPUP_ACTION_OK,
                        PopupType::POPUP_TYPE_INFO);
                }
            });
    }

    void Examples::Test_VSync_Input(
        GraphicsContext* gctx,
        Input* input)
    {
        if (input->IsComboPressed(combo_vsync_disable))
        {
            Log::Print("@@@@@ set vsync state to off");
            gctx->SetVSyncState(VSyncState::VSYNC_OFF);
        }
        else if (input->IsComboPressed(combo_vsync_adaptive))
        {
            Log::Print("@@@@@ set vsync state to adaptive");
            gctx->SetVSyncState(VSyncState::VSYNC_ON_ADAPTIVE);
        }
        else if (input->IsComboPressed(combo_vsync_triple_buffered))
        {
            Log::Print("@@@@@ set vsync state to triple buffered");
            gctx->SetVSyncState(VSyncState::VSYNC_ON_TRIPLE_BUFFERED);
        }

        if (input->IsComboPressed(combo_toggle_fps))
        {
            fpsState = !fpsState;
            string fpsStateValue = fpsState ? "on" : "off";

            Log::Print("@@@@@ set fps state to '" + fpsStateValue + "'");
        }
    }

    void Examples::Test_System_Sound_Input(Input* input)
    {
        if (input->IsComboPressed(combo_sound_ok))
        {
            Log::Print("@@@@@ played 'OK' sound");
            Window_Global::PlaySystemSound(SoundType::SOUND_OK);
        }
        else if (input->IsComboPressed(combo_sound_error))
        {
            Log::Print("@@@@@ played 'ERROR' sound");
            Window_Global::PlaySystemSound(SoundType::SOUND_ERROR);
        }
    }

    void Examples::Test_Create_Notification(Input* input)
    {
        if (input->IsComboPressed(combo_notification))
        {
            Log::Print("@@@@@ created notification");
            Window_Global::CreateNotification(
                "Metal Metropolis",
                "This is a notification test!");
        }
    }

    void Examples::Test_Get_Files(
        Input* input,
        vector<string>&& types,
        path&& requiredRoot)
    {
        if (input->IsComboPressed(combo_files_any_one))
        {
            Log::Print("@@@@@ opened file explorer with options 'single file, any'");
            vector<path> result = Window_Global::GetFiles(FileType::FILE_ANY);

            if (!result.empty()) Log::Print("@@@@@ retrieved file: " + result[0].string());
            else Log::Print("@@@@@ no file was retreived");
        }
        else if (input->IsComboPressed(combo_files_dir_mult))
        {
            Log::Print("@@@@@ opened file explorer with options 'multiple files, folders'");
            vector<path> result = Window_Global::GetFiles(
                FileType::FILE_FOLDER,
                {},
                {},
                true);

            if (!result.empty())
            {
                string resultAll{};
                for (const auto& r : result)
                {
                    resultAll += r.string() + ", ";
                }
                resultAll.pop_back();
                resultAll.pop_back();

                Log::Print("@@@@@ retrieved folders: " + resultAll);
            }
            else Log::Print("@@@@@ no folders were retreived");
        }
        else if (input->IsComboPressed(combo_files_selected_mult))
        {
            Log::Print("@@@@@ opened file explorer with options 'multiple files, custom, requiredRoot'");
            vector<path> result = Window_Global::GetFiles(
                FileType::FILE_CUSTOM,
                std::move(types),
                std::move(requiredRoot),
                true);

            if (!result.empty())
            {
                string resultAll{};
                for (const auto& r : result)
                {
                    resultAll += r.string() + ", ";
                }
                resultAll.pop_back();
                resultAll.pop_back();

                Log::Print("@@@@@ retrieved custom files: " + resultAll);
            }
            else Log::Print("@@@@@ no custom files were retreived");
        }
    }

    void Examples::Test_Window_Toggles(
        ProcessWindow* pw,
        Input* input)
    {
        if (input->IsComboPressed(combo_toggle_full_screen))
        {
            bool isFullscreen = pw->GetWindowMode() == WindowMode::WINDOWMODE_BORDERLESS;
            bool desired = !isFullscreen;

            pw->SetWindowMode(desired 
                ? WindowMode::WINDOWMODE_BORDERLESS
                : WindowMode::WINDOWMODE_WINDOWED);

            Log::Print("@@@@@ set fullscreen state to: " 
                + string(desired ? "on" : "off"));
        }

        else if (input->IsComboPressed(combo_toggle_resizable))
        {
            bool desired = !pw->IsResizable();
            pw->SetResizableState(desired);

            Log::Print("@@@@@ set resizable state to: " 
                + string(desired ? "on" : "off"));
        }
    }

    void Examples::Test_Texture_Filter_Mode(
        Input* input,
        Texture* tex)
    {
        if (input->IsComboPressed(combo_toggle_texture_filter_mode))
        {
            bool isLinear = tex->GetFilterMode() == TextureFilterMode::FILTER_LINEAR;
            bool desired = !isLinear;

            tex->SetFilterMode(desired 
                ? TextureFilterMode::FILTER_LINEAR
                : TextureFilterMode::FILTER_NEAREST);

            Log::Print("@@@@@ set texture filter mode to: " 
                + string(desired ? "linear" : "nearest"));
        }
    }

    void Examples::Test_Camera_Toggle(Input* input)
    {
        if (input->IsComboPressed(combo_toggle_cam_move))
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

    void Examples::Test_Toggle_From_Atlas_State(Input* input)
    {
        if (input->IsKeyPressed(KeyboardButton::K_BACKSPACE))
        {
            fromAtlasState = !fromAtlasState;
            string state = fromAtlasState ? "true" : "false";

            Log::Print("@@@@@ set fromAtlas state to '" + state + "'");
        }
    }
    void Examples::Test_Print_Glyph_To_Console(
        Input* input,
        ImportFont* impf)
    {
        if (!input->GetPressedKeys().empty())
        {
            u32 keycode = GetUTFByKey(scast<u32>(input->GetPressedKeys().front()));

            char keyvalue = GetValueByUTF(keycode)[0];
            if (!IsAlpha(keyvalue)
                && !IsNumber(keyvalue))
            {
                return;
            }

            Log::Print("@@@@@ key code: " + to_string(keycode));

            impf->DrawGlyphToConsole(keycode);
        }
    }
    void Examples::Test_Print_Glyph_To_Texture(
        Input* input,
        ImportFont* impf,
        Texture* glyphTexture,
        Mesh* glyphMesh)
    {
        if (!input->GetPressedKeys().empty())
        {
            u32 keycode = GetUTFByKey(scast<u32>(input->GetPressedKeys().front()));
            
            char keyvalue = GetValueByUTF(keycode)[0];
            if (!IsAlpha(keyvalue)
                && !IsNumber(keyvalue))
            {
                return;
            }

            Log::Print("@@@@@ key code: " + to_string(keycode));

            vector<u8> glyphPixelData = impf->GetGlyphPixelData(
                keycode,
                fromAtlasState);

            if (!glyphPixelData.empty()) 
            {
                GlyphData& glyphData = impf->GetGlyphData(
                    impf->GetFontData(),
                    keycode);

                vec2 finalSize = 
                {
                    fabsf(glyphData.size.x),
                    fabsf(glyphData.size.y)
                };

                glyphTexture->SetSize(finalSize);
                scast<Transform2D&>(glyphMesh->GetTransform()).setsize(finalSize * 4);

                glyphTexture->SetPixelData(std::move(glyphPixelData));
            }
        }
    }
    void Examples::Test_Print_Glyph_Atlas_To_Texture(
        ImportFont* impf,
        Texture* glyphTexture,
        Mesh* glyphMesh)
    {
        vec2 finalSize = 
        {
            fabsf(impf->GetFontData().atlasSize.x),
            fabsf(impf->GetFontData().atlasSize.y)
        };

        glyphTexture->SetSize(finalSize);
        scast<Transform2D&>(glyphMesh->GetTransform()).setsize(finalSize);

        glyphTexture->SetPixelData(vector<u8>{ impf->GetFontData().atlasPixels });
    }

    static u8 target{};
    void Examples::Test_Mesh_Toggle_Recreate_Target(Input* input)
    {
        if (input->IsMouseButtonDoubleClicked(MouseButton::M_MIDDLE))
        {
            target++;
            if (target > 2) target = 0;

            string targetStr{};
            if (target == 0) targetStr = "cube";
            else if (target == 1) targetStr = "pyramid";
            else if (target == 2) targetStr = "sphere";

            Log::Print("@@@@@ selected target: " + targetStr);
        }
    }

    void Examples::Test_Mesh_Recreate_Cube_On_Mouse_Actions(
        Input* input,
        Mesh* mesh)
    {
        if (mesh->Is2D())
        {
            Log::Print(
                "Cannot recreate mesh '" 
                + to_string(mesh->GetID()) + "' cube data because it is 2D!");

            return;
        }
        if (target != 0) return;

        static Mesh_Cube cubeData{ .edgeCount = 4 };

        if (input->GetScrollwheelDelta() != 0.0f)
        {
            if (input->GetScrollwheelDelta() > 0.0f)
            {
                if (cubeData.edgeCount == 32) return;
                ++cubeData.edgeCount;
            }
            else if (input->GetScrollwheelDelta() < 0.0f)
            {
                if (cubeData.edgeCount == 3) return;
                --cubeData.edgeCount;
            }

            mesh->SetMeshData(Mesh::GenerateMeshData(cubeData));
        }

        if (input->IsMouseButtonDoubleClicked(MouseButton::M_LEFT))
        {
            cubeData.faceDir = cubeData.faceDir == FaceDirection::F_IN
                ? FaceDirection::F_OUT
                : FaceDirection::F_IN;

            mesh->SetMeshData(Mesh::GenerateMeshData(cubeData));
        }
        else if (input->IsMouseButtonDoubleClicked(MouseButton::M_RIGHT))
        {
            cubeData.normalType = cubeData.normalType == NormalType::N_FLAT
                ? NormalType::N_SMOOTH
                : NormalType::N_FLAT;

            mesh->SetMeshData(Mesh::GenerateMeshData(cubeData));
        }
    }
    void Examples::Test_Mesh_Recreate_Pyramid_On_Mouse_Actions(
        Input* input,
        Mesh* mesh)
    {
        if (mesh->Is2D())
        {
            Log::Print(
                "Cannot recreate mesh '" 
                + to_string(mesh->GetID()) + "' pyramid data because it is 2D!");

            return;
        }
        if (target != 1) return;

        static Mesh_Pyramid pyramidData{ .edgeCount = 4 };

        if (input->GetScrollwheelDelta() != 0.0f)
        {
            if (input->GetScrollwheelDelta() > 0.0f)
            {
                if (pyramidData.edgeCount == 32) return;
                ++pyramidData.edgeCount;
            }
            else if (input->GetScrollwheelDelta() < 0.0f)
            {
                if (pyramidData.edgeCount == 3) return;
                --pyramidData.edgeCount;
            }

            mesh->SetMeshData(Mesh::GenerateMeshData(pyramidData));
        }

        if (input->IsMouseButtonDoubleClicked(MouseButton::M_LEFT))
        {
            pyramidData.faceDir = pyramidData.faceDir == FaceDirection::F_IN
                ? FaceDirection::F_OUT
                : FaceDirection::F_IN;

            mesh->SetMeshData(Mesh::GenerateMeshData(pyramidData));
        }
        else if (input->IsMouseButtonDoubleClicked(MouseButton::M_RIGHT))
        {
            pyramidData.normalType = pyramidData.normalType == NormalType::N_FLAT
                ? NormalType::N_SMOOTH
                : NormalType::N_FLAT;

            mesh->SetMeshData(Mesh::GenerateMeshData(pyramidData));
        }
    }
    void Examples::Test_Mesh_Recreate_Sphere_On_Mouse_Actions(
        Input* input,
        Mesh* mesh)
    {
        if (mesh->Is2D())
        {
            Log::Print(
                "Cannot recreate mesh '" 
                + to_string(mesh->GetID()) + "' sphere data because it is 2D!");

            return;
        }
        if (target != 2) return;

        static Mesh_Sphere sphereData{};

        if (input->GetScrollwheelDelta() != 0.0f)
        {
            if (input->GetScrollwheelDelta() > 0.0f)
            {
                if (sphereData.detailLevel == 8) return;
                ++sphereData.detailLevel;
            }
            else if (input->GetScrollwheelDelta() < 0.0f)
            {
                if (sphereData.detailLevel == 1) return;
                --sphereData.detailLevel;
            }

            mesh->SetMeshData(Mesh::GenerateMeshData(sphereData));
        }

        if (input->IsMouseButtonDoubleClicked(MouseButton::M_LEFT))
        {
            sphereData.faceDir = sphereData.faceDir == FaceDirection::F_IN
                ? FaceDirection::F_OUT
                : FaceDirection::F_IN;

            mesh->SetMeshData(Mesh::GenerateMeshData(sphereData));
        }
        else if (input->IsMouseButtonDoubleClicked(MouseButton::M_RIGHT))
        {
            sphereData.normalType = sphereData.normalType == NormalType::N_FLAT
                ? NormalType::N_SMOOTH
                : NormalType::N_FLAT;

            mesh->SetMeshData(Mesh::GenerateMeshData(sphereData));
        }
    }

    void Examples::Test_Camera_Move(
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

    Shader* Examples::Test_Create_Shader(
        Viewport* viewport,
        bool is2D,
        array<path, 2>&& shaderFiles)
    {
        //sync ids before generating shader
        EngineCore::SyncID();

        Shader* shader = Shader::Initialize(
            viewport->GetID(),
            is2D,
            path(shaderFiles[0]),
            path(shaderFiles[1]));

        if (!shader)
        {
            KalaWindowCore::ForceClose(
                "Game core error",
                "Failed to initialize test shader!");
        }

        return shader;
    }

    Texture* Examples::Test_Create_Texture(
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

    Mesh* Examples::Test_Create_Mesh(
        Shader* shader,
        Texture* texture,
        MeshData&& meshData)
    {
        //sync ids before generating mesh
        EngineCore::SyncID();

        Mesh* mesh = Mesh::Initialize(
            shader->GetID(),
            texture->GetID());
        if (!mesh)
        {
            KalaWindowCore::ForceClose(
                "Game core error",
                "Failed to initialize test mesh!");
        }

        if (!mesh->Is2D()) mesh->SetMeshData(std::move(meshData));

        return mesh;
    }

    Camera* Examples::Test_Create_Camera(Shader* shader)
    {
        //sync ids before generating camera
        EngineCore::SyncID();

        Camera* cam = Camera::Initialize(shader->GetID());
        if (!cam)
        {
            KalaWindowCore::ForceClose(
                "Game core error",
                "Failed to initialize test camera!");
        }

        return cam;
    }
}