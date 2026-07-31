//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <format>
#include <chrono>

#include "log_utils.hpp"
#include "key_standards.hpp"

#include "test/test_examples.hpp"
#include "core/ee_core.hpp"
#include "core/kg_context.hpp"
#include "core/kw_core.hpp"
#include "resources/kg_shader.hpp"
#include "resources/kg_mesh.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaKeyStandards::KeyboardButton;

using ElypsoEngine::Core::EngineCore;
using KalaGraphics::Core::VSyncState;
using KalaWindow::Core::KalaWindowCore;
using KalaGraphics::Resources::Shader;
using KalaGraphics::Resources::Mesh;

using std::string;
using std::format;
using std::chrono::time_point;
using std::chrono::steady_clock;
using std::chrono::seconds;
using std::chrono::duration;
using std::format;

namespace MetalMetropolis::Test
{
    string Examples::GetFPS(f64 secondsToWait)
    {
        static f64 cachedFPS = EngineCore::GetCurrentFPS();
        static time_point start = steady_clock::now();

        if (steady_clock::now() - start > duration<f64>(secondsToWait))
        {
            cachedFPS = EngineCore::GetCurrentFPS();
            start = steady_clock::now();
        }
        
        return format("{:.2f}", cachedFPS);
    }

    void Examples::Test_Input(
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