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
#include "resources/kg_shader.hpp"
#include "resources/kg_mesh.hpp"
#include "graphics/kw_window_global.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaMath::vec2;

using KalaHeaders::KalaKeyStandards::KeyboardButton;

using ElypsoEngine::Core::EngineCore;
using KalaGraphics::Core::VSyncState;
using KalaWindow::Core::KalaWindowCore;
using KalaGraphics::Resources::Shader;
using KalaGraphics::Resources::Mesh;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::SoundType;
using KalaWindow::Graphics::FileType;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupType;

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

        if (input->IsKeyPressed(KeyboardButton::K_4))
        {
            fpsState = !fpsState;
            string fpsStateValue = fpsState ? "on" : "off";

            Log::Print("@@@@@ set fps state to '" + fpsStateValue + "'");
        }
    }

    void Examples::Test_System_Sound_Input(Input* input)
    {
        if (input->IsKeyPressed(KeyboardButton::K_5))
        {
            Log::Print("@@@@@ played 'OK' sound");
            Window_Global::PlaySystemSound(SoundType::SOUND_OK);
        }
        if (input->IsKeyPressed(KeyboardButton::K_6))
        {
            Log::Print("@@@@@ played 'ERROR' sound");
            Window_Global::PlaySystemSound(SoundType::SOUND_ERROR);
        }
    }

    void Examples::Test_Create_Notification(Input* input)
    {
        if (input->IsKeyPressed(KeyboardButton::K_7))
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
        if (input->IsKeyPressed(KeyboardButton::K_8))
        {
            Log::Print("@@@@@ opened file explorer with options 'single file, any'");
            vector<path> result = Window_Global::GetFiles(FileType::FILE_ANY);

            if (!result.empty()) Log::Print("@@@@@ retrieved file: " + result[0].string());
            else Log::Print("@@@@@ no file was retreived");
        }
        if (input->IsKeyPressed(KeyboardButton::K_9))
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
        if (input->IsKeyPressed(KeyboardButton::K_0))
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

    void Examples::Create_Triangle(
        GraphicsContext* gctx,
        Transform&& triangleTransform,
        vector<Vertex>&& triangleVertices,
        array<path, 2>&& triangleShaders)
    {
        //sync ids before generating shader
        EngineCore::SyncID();

        Shader* shader = Shader::Initialize(
            gctx->GetID(),
            {
                .shader_vert = triangleShaders[0],
                .shader_frag = triangleShaders[1]
            });

        if (!shader)
        {
            KalaWindowCore::ForceClose(
                "Game core error",
                "Failed to initialize shader 'shader-test'!");
        }

        //sync ids before generating mesh
        EngineCore::SyncID();

        Mesh* mesh = Mesh::Initialize(
            true,
            gctx->GetID(),
            shader->GetID(),
            std::move(triangleTransform),
            std::move(triangleVertices),
            {});

        if (!mesh)
        {
            KalaWindowCore::ForceClose(
                "Game core error",
                "Failed to initialize mesh 'mesh-test'!");
        }
    }
}