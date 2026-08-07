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
#include "key_standards.hpp"

#include "test/test_examples.hpp"
#include "core/ee_core.hpp"
#include "core/kg_context.hpp"
#include "core/kw_core.hpp"
#include "graphics/kw_window_global.hpp"
#include "resources/kg_shader.hpp"
#include "resources/kg_mesh.hpp"
#include "resources/kg_camera.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaMath::Transform3D;
using KalaHeaders::KalaMath::vec2;
using KalaHeaders::KalaMath::setpos3d;
using KalaHeaders::KalaMath::setroteuler;
using KalaHeaders::KalaMath::setsize3d;
using KalaHeaders::KalaMath::PosTarget;
using KalaHeaders::KalaMath::RotTarget;
using KalaHeaders::KalaMath::SizeTarget;

using KalaHeaders::KalaKeyStandards::KeyboardButton;

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
using KalaGraphics::Resources::Shader;
using KalaGraphics::Resources::Mesh;
using KalaGraphics::Resources::Camera;
using KalaGraphics::Resources::CameraType;

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

namespace MetalMetropolis::Test
{
    string Examples::GetFPS(f64 secondsToWait)
    {
        if (!fpsState) return "";

        static f64 cachedFPS = EngineCore::GetCurrentFPS();
        static time_point start = steady_clock::now();

        if (steady_clock::now() - start > duration<f64>(secondsToWait))
        {
            cachedFPS = EngineCore::GetCurrentFPS();
            start = steady_clock::now();
        }
        
        return format("{:.2f}", cachedFPS);
    }

    void Examples::Test_Popup_And_File_Drag(ProcessWindow* pw)
    {
        pw->SetDraggedFilesCallback([](const vector<path>& files, vec2 pos)
            {
                if (files.empty())
                {
                    Window_Global::CreatePopup(
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

                    Window_Global::CreatePopup(
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
        Input *input,
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
        ProcessWindow *pw,
        Input *input)
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

    void Examples::Create_Triangle(
        GraphicsContext* gctx,
        Transform&& triangleTransform,
        vector<Vertex>&& triangleVertices,
        array<path, 2>&& triangleShaders)
    {
        //sync ids before generating shader
        EngineCore::SyncID();

        Shader* shader = Shader::Initialize(gctx->GetID());
        if (!shader)
        {
            KalaWindowCore::ForceClose(
                "Game core error",
                "Failed to initialize shader 'shader-test'!");
        }

        shader->SetShaderData(
            {
                .shader_vert = triangleShaders[0],
                .shader_frag = triangleShaders[1]
            });

        //sync ids before generating mesh
        EngineCore::SyncID();

        Mesh* mesh = Mesh::Initialize(shader->GetID());
        if (!mesh)
        {
            KalaWindowCore::ForceClose(
                "Game core error",
                "Failed to initialize mesh 'mesh-test'!");
        }

        mesh->GetVertices() = std::move(triangleVertices);

        Transform3D meshTransform = mesh->GetTransform();

        setpos3d(
            meshTransform,
            {},
            PosTarget::POS_WORLD,
            triangleTransform.pos);
        setroteuler(
            meshTransform,
            {}, 
            RotTarget::ROT_WORLD,
            triangleTransform.rot);
        setsize3d(
            meshTransform,
            {},
            SizeTarget::SIZE_WORLD,
            triangleTransform.size);

        Camera* cam = Camera::Initialize(
            gctx->GetID(),
            shader->GetID());
        cam->SetCameraType(CameraType::C_ORTHOGRAPHIC);
    }
}